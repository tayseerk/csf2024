#include "cache.h"

Cache::Cache(unsigned int numberOfSets, unsigned int blocksPerSet, unsigned int bytesPerBlock,
             const std::string& writeAllocate, const std::string& writeThrough,
             const std::string& eviction)
    : numberOfSets(numberOfSets),
      blocksPerSet(blocksPerSet),
      bytesPerBlock(bytesPerBlock),
      writeAllocate(writeAllocate),
      writeThrough(writeThrough),
      cacheSets(), // default initialization
      totalLoads(0),
      totalStores(0),
      loadHits(0),
      loadMisses(0),
      storeHits(0),
      storeMisses(0),
      totalCycles(0),
      eviction(eviction) // eviction has to be initialized after totalCycles
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
    // implement later
    ++totalLoads;
}

void Cache::store(unsigned int address) {
    (void)address; // to prevent unused param warning
    // implement later
    ++totalStores;
}

unsigned int Cache::findSetIndex(unsigned int address) const {
    // implement later - calculate set index using address
    // placeholder return 
    return (address / bytesPerBlock) % numberOfSets;
}

unsigned int Cache::findTag(unsigned int address) const {
    // implement later - calculate tag using address
    // placeholder return
    return (address / bytesPerBlock) / numberOfSets;
}

int Cache::findBlockWithinSet(unsigned int setIndex, unsigned int tag) {
    (void)setIndex; // to prevent unused param warning
    (void)tag; // to prevent unused param warning
    // implement later - searching for the block within a set using a tag
    return -1; // not found (placeholder for now)
}

int Cache::chooseBlockToEvict(unsigned int setIndex) {
    (void)setIndex; // to prevent unused param warning
    // implement later - find a block to evict based on the method
    return 0; // index of victim block (placeholder for now)
}

