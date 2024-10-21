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
    if (!found) { // block not in cache
        ++loadMisses;
        loadToCache(address);
        totalCycles += 100;
    } else { // block is in cache
        ++loadHits;
        ++totalCycles;
    }
    ++totalLoads;
}

void Cache::loadToCache(unsigned int address) {
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int slotIndex = getFreeIndex(setIndex);
    cacheSets[setIndex][slotIndex].valid = true; // make the block valid
    cacheSets[setIndex][slotIndex].tag = tag; // change the tag to this block's
    cacheSets[setIndex][slotIndex].timestamp = -1; // set to -1 (will increment to 0)
    incrementTimeStamps(setIndex); // increment every time stamp in a set
}

void Cache::store(unsigned int address) {
    (void)address; // to prevent unused param warning

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
}

void Cache::storeToCache(unsigned int address){
    unsigned int setIndex = findSetIndex(address);
    unsigned int tag = findTag(address);
    int slotIndex = getFreeIndex(setIndex);
    if (writeAllocate == "write-allocate"){
        cacheSets[setIndex][slotIndex].valid = true; // make the block valid
        cacheSets[setIndex][slotIndex].tag = tag; // change the tag to this block's
        cacheSets[setIndex][slotIndex].timestamp = -1; // set to -1 (will increment to 0)
        incrementTimeStamps(setIndex); // increment every time stamp in a set
    }
    if (writeThrough == "write-back" && writeAllocate != "write-back"){
        cacheSets[setIndex][slotIndex].dirty = true;
    } else {
        //error
    }

}

int Cache::getFreeIndex(unsigned int setIndex){
    int slotIndex; // index of slot to put block in
    int i = emptySlot(setIndex); // temporary variable for finding empty slot
    if (i == -1) { // if the set has no more empty slots
        slotIndex = chooseBlockToEvict(setIndex); // evict a block
    } else {
        slotIndex = i; // empty slot found = slot for block
    }
    return slotIndex;
}

int Cache::emptySlot(unsigned int setIndex){
    for (unsigned int i = 0; i < blocksPerSet; i++){ // for every slot within the set
        if(!cacheSets[setIndex][i].valid){ // if slot is not valid == slot is empty
            return i; // set isn't full
        }
    }
    return -1; // no empty slot found
}

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

unsigned int Cache::findSetIndex(unsigned int address) const {
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
    for (unsigned int i = 0; i < blocksPerSet; i++){
        if (cacheSets[setIndex][i].tag == tag){
            return 0; // found
        }
    }
    return -1; // not found
}

int Cache::chooseBlockToEvict(unsigned int setIndex) {
    (void)setIndex; // to prevent unused param warning
    if (eviction == "lru"){
        int slotIndex = findLeastRecentlyUsed(setIndex);
        return slotIndex;
    } else { //FIFO implement later
        return 0; // index of victim block (placeholder for now)
    }
}

int Cache::findLeastRecentlyUsed(unsigned int setIndex){
    int leastRecent = 0;
    for (unsigned int i = 1; i < blocksPerSet; i++){
        if(cacheSets[setIndex][leastRecent].timestamp < cacheSets[setIndex][i].timestamp){
            leastRecent = i;
        }
    }
    return leastRecent;
}

void Cache::incrementTimeStamps(unsigned int setIndex){
        for (unsigned int i = 0; i < blocksPerSet; i++){
            cacheSets[setIndex][i].timestamp++;
    }
}

