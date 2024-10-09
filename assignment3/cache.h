#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <string>

struct Block {
    bool valid;
    unsigned int tag;
    bool dirty;
    unsigned int timestamp; // for LRU/FIFO
};

// Initialize a cache -- sets and blocks
// 
// Parameters:
//  numberOfSets - sets in cache
//  blocksPerSet - blocks in each set
//  bytesPerBlock - bytes in each block
//  writeAllocate - string for write allocate
//  writeThrough - string for writeThrough
//  eviction - string for eviction
//
// Returns
//  Cache object
class Cache {
public:
    Cache(unsigned int numberOfSets, unsigned int blocksPerSet, unsigned int bytesPerBlock, 
            const std::string& writeAllocate, const std::string& writeThrough, const std::string& eviction);

    // Accesses the cache at a specific address
    // with a specific access command
    // 
    // Parameters:
    //  accessType - string for type of access (l for load, s for store)
    //  address - address to access
    //
    void accessCache(const std::string& accessType, unsigned int address);

    // cache statistics
    unsigned int totalLoads;
    unsigned int totalStores;
    unsigned int loadHits;
    unsigned int loadMisses;
    unsigned int storeHits;
    unsigned int storeMisses;
    unsigned int totalCycles;

private:

    unsigned int numberOfSets;
    unsigned int blocksPerSet;
    unsigned int bytesPerBlock;
    std::string writeAllocate;
    std::string writeThrough;
    std::string eviction;
    std::vector<std::vector<Block>> cacheSets;

    // Loads an address in the cache and 
    // increments totalLoads 
    // 
    // Parameters:
    //  address - address to load
    //
    void load(unsigned int address);

    // Stores an address in the cache and 
    // increments totalStores 
    // 
    // Parameters:
    //  address - address to store
    //
    void store(unsigned int address);

    // Calculates an index for a set using its address
    //
    // Parameters:
    //  address - address to find
    //
    // Returns:
    //  the index of the address
    //
    unsigned int findSetIndex(unsigned int address) const;

    // Calculates a tag for a block using its address
    //
    // Parameters:
    //  address - address to find
    //
    // Returns:
    //  the tag of the address
    //
    unsigned int findTag(unsigned int address) const;

    // Searches for a block inside of a set using a tag
    //
    // Parameters:
    //  setIndex - the index of a set
    //  tag - the tag of a block
    //
    // Returns:
    //  -1 if not found
    //   0 if the block is found
    //
    int findBlockWithinSet(unsigned int setIndex, unsigned int tag);

    // Selects the block that needs to be evicted
    //
    // Parameters:
    //  setIndex - the index of a set
    //  
    // Returns:
    //  the index of the block to be evicted
    //
    int chooseBlockToEvict(unsigned int setIndex);
};

#endif // CACHE_H
