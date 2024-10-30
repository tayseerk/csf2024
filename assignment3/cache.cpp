#include "cache.h"
#include <cmath>
#include <iostream>

Cache::Cache(unsigned int numberOfSets, unsigned int blocksPerSet, unsigned int bytesPerBlock,
             const std::string& writeAllocate, const std::string& writeThrough,
             const std::string& eviction)
    : totalLoads(0),
      totalStores(0),
      loadHits(0),
      loadMisses(0),
      storeHits(0),
      storeMisses(0),
      totalCycles(0),
      numberOfSets(numberOfSets),
      blocksPerSet(blocksPerSet),
      bytesPerBlock(bytesPerBlock),
      writeAllocate(writeAllocate),
      writeThrough(writeThrough),
      eviction(eviction),
      cacheSets(), // default initialization
      currTimestamp(0)
{
    // initialize cache sets and blocks
    cacheSets.resize(numberOfSets, std::vector<Block>(blocksPerSet));

    // do it for each block
    for (unsigned int x = 0; x < numberOfSets; ++x) {
        for (unsigned int y = 0; y < blocksPerSet; ++y) {
            cacheSets[x][y].valid = false;
            cacheSets[x][y].tag = 0;
            cacheSets[x][y].dirty = false;
            cacheSets[x][y].timestamp = 0; // for lru/fifo
        }
    }
}

void Cache::accessCache(const std::string& accessType, unsigned int address) {
    if (accessType == "l") {
        load(address);
    } else if (accessType == "s") {
        store(address);
    }
}

void Cache::load(unsigned int address) {
    ++totalLoads;
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int blockIndex = findBlockWithinSet(setIndex, tag);

    if (blockIndex != -1) {
        ++loadHits; // hit
        totalCycles += 1;
        if (eviction == "lru") {
            updateLRU(setIndex, blockIndex); // update 
        }
        // nothing needs to be done for fifo when hit
    } else {
        ++loadMisses; // miss
        loadToCache(address);
        totalCycles += (bytesPerBlock / 4) * 100; // mem access cost
        totalCycles += 1; // access cache after
    }
}

void Cache::loadToCache(unsigned int address) {
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int slotIndex = getFreeIndex(setIndex);

    // load into cache
    cacheSets[setIndex][slotIndex].valid = true;
    cacheSets[setIndex][slotIndex].tag = tag;
    cacheSets[setIndex][slotIndex].dirty = false;

    // for lru
    if (eviction == "lru") {
        cacheSets[setIndex][slotIndex].timestamp = 0;
        updateLRU(setIndex, slotIndex);
    } else if (eviction == "fifo") {
        cacheSets[setIndex][slotIndex].timestamp = currTimestamp++; // for fifo
    }
}

void Cache::store(unsigned int address) {
    ++totalStores;
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int blockIndex = findBlockWithinSet(setIndex, tag);

    if (blockIndex != -1) {
        ++storeHits; // hit
        totalCycles += 1;
        if (eviction == "lru") {
            updateLRU(setIndex, blockIndex); // update
        }
        // nothing needs to be done for fifo when hit
        if (writeThrough == "write-through") {
            totalCycles += 100; // write to mem
        } else if (writeThrough == "write-back") {
            cacheSets[setIndex][blockIndex].dirty = true; // dirty block
        }
    } else {
        ++storeMisses; // miss
        if (writeAllocate == "write-allocate") {
            storeToCache(address);
            totalCycles += (bytesPerBlock / 4) * 100; // mem access cost
            totalCycles += 1; // access cache after 
        } else if (writeAllocate == "no-write-allocate") {
            if (writeThrough == "write-through") {
                totalCycles += 100; // write to mem
            } else {
                std::cerr << "can't do both commands" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

void Cache::storeToCache(unsigned int address){
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int slotIndex = getFreeIndex(setIndex);

    // load into cache
    cacheSets[setIndex][slotIndex].valid = true;
    cacheSets[setIndex][slotIndex].tag = tag;
    cacheSets[setIndex][slotIndex].dirty = false;

    if (eviction == "lru") {
        cacheSets[setIndex][slotIndex].timestamp = 0;
        updateLRU(setIndex, slotIndex);
    } else if (eviction == "fifo") {
        cacheSets[setIndex][slotIndex].timestamp = currTimestamp++; // for fifo
    }

    if (writeThrough == "write-back") {
        cacheSets[setIndex][slotIndex].dirty = true;
    } else if (writeThrough == "write-through") {
        totalCycles += 100; // write to mem
    }
}

int Cache::getFreeIndex(unsigned int setIndex) {
    int i = emptySlot(setIndex);
    if (i == -1) {
        i = chooseBlockToEvict(setIndex); // evict
        // write back dirty block if necessary
        if (writeThrough == "write-back" && cacheSets[setIndex][i].dirty) {
            totalCycles += (bytesPerBlock / 4) * 100; 
            cacheSets[setIndex][i].dirty = false;
        }
        cacheSets[setIndex][i].valid = false;
    }
    return i;
}

int Cache::emptySlot(unsigned int setIndex){
    for (unsigned int i = 0; i < blocksPerSet; i++) { // for every slot within the set
        if (!cacheSets[setIndex][i].valid) { // if slot is not valid == slot is empty
            return i; // set isn't full
        }
    }
    return -1; // no empty slot found
}

int Cache::findBlock(unsigned int address) const {
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    return findBlockWithinSet(setIndex, tag);
}

unsigned int Cache::findSetIndex(unsigned int address) const {
    unsigned int offset = std::log2(bytesPerBlock);
    unsigned int indexBits = std::log2(numberOfSets);
    unsigned int setIndex = (address >> offset) & ((1 << indexBits) - 1);
    return setIndex;
}

unsigned int Cache::findTag(unsigned int address) const {
    unsigned int offset = std::log2(bytesPerBlock);
    unsigned int indexBits = std::log2(numberOfSets);
    unsigned int tag = address >> (offset + indexBits);
    return tag;
}

int Cache::findBlockWithinSet(unsigned int setIndex, unsigned int tag) const {
    for (unsigned int i = 0; i < blocksPerSet; i++) {
        if (cacheSets[setIndex][i].valid && cacheSets[setIndex][i].tag == tag) {
            return i;
        }
    }
    return -1;
}

int Cache::chooseBlockToEvict(unsigned int setIndex) {
    if (eviction == "lru") {
        return findLeastRecentlyUsed(setIndex);
    } else if (eviction == "fifo") {
        return findFirstIn(setIndex); // use fifo
    } else {
        std::cerr << "invalid eviction policy" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int Cache::findLeastRecentlyUsed(unsigned int setIndex) {
    int lruIndex = 0;
    unsigned int maxTimestamp = cacheSets[setIndex][0].timestamp;
    for (unsigned int i = 1; i < blocksPerSet; i++) {
        if (cacheSets[setIndex][i].timestamp > maxTimestamp) {
            maxTimestamp = cacheSets[setIndex][i].timestamp;
            lruIndex = i;
        }
    }
    return lruIndex;
}

void Cache::updateLRU(unsigned int setIndex, int accessedIndex) {
    for (unsigned int i = 0; i < blocksPerSet; i++) {
        if (cacheSets[setIndex][i].valid && int(i) != accessedIndex) {
            cacheSets[setIndex][i].timestamp++;
        }
    }
    cacheSets[setIndex][accessedIndex].timestamp = 0;
}

int Cache::findFirstBlockIn(unsigned int setIndex) {
    int index = 0;
    unsigned int minTimestamp = cacheSets[setIndex][0].timestamp;
    for (unsigned int i = 1; i < blocksPerSet; i++) {
        if (cacheSets[setIndex][i].timestamp < minTimestamp) {
            minTimestamp = cacheSets[setIndex][i].timestamp;
            index = i;
        }
    }
    return index;
}
