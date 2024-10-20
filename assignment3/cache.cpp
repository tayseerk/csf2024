#include "cache.h"
#include <cmath>

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
    (void)address; // to prevent unused param warning

    bool found = findBlock(address); // if block is in cache
    if (!found) { //block not in cache
        ++loadMisses;
        // implemet load into cache
        totalCycles += 100;
    } else { //block is in cache
        ++loadHits;
        ++totalCycles;
    }
    ++totalLoads;
}

void Cache::store(unsigned int address) {
    (void)address; // to prevent unused param warning

    int found = findBlock(address); // if block is in cache
    if (!found) { //block not in cache
        ++storeMisses;
        //implement store into cache
        totalCycles += 100;
    } else { //block is in cache
        ++storeHits;
        ++totalCycles;
    }
    ++totalStores;
}

bool Cache::findBlock(unsigned int address) const {
    unsigned int index = findSetIndex(address);
    unsigned int tag = findTag(address);
    int block = findBlockWithinSet(index, tag);
    if (block == -1){
        return false;
    } else {
        return true;
    }
}

unsigned int Cache::findSetIndex(unsigned int address) const {
    // implement later - calculate set index using address
    // not sure if this is right
    unsigned int blockOffset = log2(bytesPerBlock);
    unsigned int indexBits = log2(numberOfSets*blocksPerSet);
    unsigned int index = (address >> blockOffset) & ((1 << indexBits) - 1);
    return index;
    
}

unsigned int Cache::findTag(unsigned int address) const {
    // not sure if this is right
    unsigned int blockOffset = log2(bytesPerBlock);
    unsigned int indexBits = log2(numberOfSets*blocksPerSet);
    unsigned int tag = address >> (blockOffset + indexBits);
    return tag;
}

int Cache::findBlockWithinSet(unsigned int setIndex, unsigned int tag) const {
    (void)setIndex; // to prevent unused param warning
    (void)tag; // to prevent unused param warning
    if (cacheSets[setIndex][tag].valid){
        return 0;
    } else{
    return -1; // not found
    }
}

int Cache::chooseBlockToEvict(unsigned int setIndex) {
    (void)setIndex; // to prevent unused param warning
    // implement later - find a block to evict based on the method
    return 0; // index of victim block (placeholder for now)
}

