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

// finish

#endif // CACHE_H
