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
      cacheSets() // default initialization
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
        ++loadHits; // cache hit
        totalCycles += 1;
        updateLRU(setIndex, blockIndex); // update
    } else {
        ++loadMisses; // cache miss
        loadToCache(address);
        totalCycles += (bytesPerBlock / 4) * 100; // mem access cost
        totalCycles += 1; // cache access after loading
    }
}

void Cache::loadToCache(unsigned int address) {
    /*
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int slotIndex = getFreeIndex(setIndex);
    cacheSets[setIndex][slotIndex].valid = true; // make the block valid
    cacheSets[setIndex][slotIndex].tag = tag; // change the tag to this block's
    cacheSets[setIndex][slotIndex].timestamp = -1; // set to -1 (will increment to 0)
    incrementTimeStamps(setIndex); // increment every time stamp in a set
    */
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int slotIndex = getFreeIndex(setIndex);

    // load into cache
    cacheSets[setIndex][slotIndex].valid = true;
    cacheSets[setIndex][slotIndex].tag = tag;
    cacheSets[setIndex][slotIndex].dirty = false;
    cacheSets[setIndex][slotIndex].timestamp = 0;
  
    updateLRU(setIndex, slotIndex);
}

void Cache::store(unsigned int address) {
    /*
    int found = findBlock(address); // if block is in cache
    if (!found) { //block not in cache
        ++storeMisses;
        //implement store into cache
        storeToCache(address);
        totalCycles += 100;
    } else { //block is in cache
        // mark block dirty?
        ++storeHits;
        ++totalCycles;
    }
    ++totalStores;
    */
    ++totalStores;

    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int blockIndex = findBlockWithinSet(setIndex, tag);

    if (blockIndex != -1) {
        ++storeHits; // hit
        totalCycles += 1;
        updateLRU(setIndex, blockIndex);

        if (writeThrough == "write-through") {
            totalCycles += 100; // write to mem
        } else if (writeThrough == "write-back") {
            cacheSets[setIndex][blockIndex].dirty = true; // dirty block
        }
    } else {
        ++storeMisses; // miss
        if (writeAllocate == "write-allocate") {
            storeToCache(address); // load 
            totalCycles += (bytesPerBlock / 4) * 100; // mem access cost
            totalCycles += 1; 
        } else if (writeAllocate == "no-write-allocate") {
            if (writeThrough == "write-through") {
                totalCycles += 100; // write to mem
            } else {
                std::cerr << "error: can't do both no-write-allocate and write-back." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

void Cache::storeToCache(unsigned int address){
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int slotIndex = getFreeIndex(setIndex);

    // load
    cacheSets[setIndex][slotIndex].valid = true;
    cacheSets[setIndex][slotIndex].tag = tag;
    cacheSets[setIndex][slotIndex].dirty = false;
    cacheSets[setIndex][slotIndex].timestamp = 0;
    updateLRU(setIndex, slotIndex);

    if (writeThrough == "write-back") {
        cacheSets[setIndex][slotIndex].dirty = true; // dirty block
    } else if (writeThrough == "write-through") {
        totalCycles += 100; // write to mem
    }
}

int Cache::getFreeIndex(unsigned int setIndex) {
    /*
    int slotIndex; // index of slot to put block in
    int i = emptySlot(setIndex); // temporary variable for finding empty slot
    if (i == -1) { // if the set has no more empty slots
        slotIndex = chooseBlockToEvict(setIndex); // evict a block
    } else {
        slotIndex = i; // empty slot found = slot for block
    }
    return slotIndex;
    */
    int slotIndex = emptySlot(setIndex);
    if (slotIndex == -1) {
        slotIndex = chooseBlockToEvict(setIndex); // evict
        // write back dirty block if necessary
        if (writeThrough == "write-back" && cacheSets[setIndex][slotIndex].dirty) {
            totalCycles += (bytesPerBlock / 4) * 100; 
            cacheSets[setIndex][slotIndex].dirty = false;
        }
        cacheSets[setIndex][slotIndex].valid = false;
    }
    return slotIndex;
}

int Cache::emptySlot(unsigned int setIndex){
    for (unsigned int i = 0; i < blocksPerSet; i++) { // for every slot within the set
        if (!cacheSets[setIndex][i].valid) { // if slot is not valid == slot is empty
            return i; // set isn't full
        }
    }
    return -1; // no empty slot found
}

/*
  bool Cache::findBlock(unsigned int address) const {
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int block = findBlockWithinSet(setIndex, tag); 
    if (block == -1){ // block not in cache
        return false;
    } else {
        return true; // block is in cache
    }
}
*/

int Cache::findBlock(unsigned int address) const {
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    return findBlockWithinSet(setIndex, tag);
}

unsigned int Cache::findSetIndex(unsigned int address) const {
    unsigned int blockOffsetBits = std::log2(bytesPerBlock);
    unsigned int indexBits = std::log2(numberOfSets);
    unsigned int setIndex = (address >> blockOffsetBits) & ((1 << indexBits) - 1);
    return setIndex;
}

unsigned int Cache::findTag(unsigned int address) const {
    unsigned int blockOffsetBits = std::log2(bytesPerBlock);
    unsigned int indexBits = std::log2(numberOfSets);
    unsigned int tag = address >> (blockOffsetBits + indexBits);
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
    } else {
        return 0; // placeholder 
    }
}

int Cache::findLeastRecentlyUsed(unsigned int setIndex) {
    /*
    int leastRecent = 0;
    for (unsigned int i = 1; i < blocksPerSet; i++){
        if(cacheSets[setIndex][leastRecent].timestamp < cacheSets[setIndex][i].timestamp){
            leastRecent = i;
        }
    }
    return leastRecent;
}
*/
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

/* timestamp isn't being incremented properly
void Cache::incrementTimeStamps(unsigned int setIndex){
        for (unsigned int i = 0; i < blocksPerSet; i++){
            cacheSets[setIndex][i].timestamp++;
    }
}
*/

