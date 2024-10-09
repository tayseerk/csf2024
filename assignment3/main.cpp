#include <iostream>
#include <string>
#include <cstdlib>
#include "cache.h"

void printUsage(const char* program) {
    std::cerr << "Usage: " << program
              << " <number_of_sets> <blocks_per_set> <bytes_per_block> "
              << "<write-allocate|no-write-allocate> "
              << "<write-through|write-back> <lru|fifo>" << std::endl;
    exit(EXIT_FAILURE);
}

bool checkForPowerOfTwo(unsigned int x) {
    return x && !(x & (x - 1));
}

int main(int argc, char* argv[]) {
    if (argc != 7) {
        printUsage(argv[0]);
    }

    // parse necessary params
    unsigned int numberOfSets = std::atoi(argv[1]);
    unsigned int blocksPerSet = std::atoi(argv[2]);
    unsigned int bytesPerBlock = std::atoi(argv[3]);

    std::string writeAllocate = argv[4];
    std::string write = argv[5];
    std::string eviction = argv[6];

    // check if cache params is valid  
    if (!checkForPowerOfTwo(numberOfSets) || !checkForPowerOfTwo(blocksPerSet) ||
        !checkForPowerOfTwo(bytesPerBlock) || bytesPerBlock < 4) {
        std::cerr << "Error: cache parameters are invalid." << std::endl;
        exit(EXIT_FAILURE);
    }
  
    bool checkIfWriteAllocate;
    if (writeAllocate == "write-allocate") {
        checkIfWriteAllocate = true;
    } else if (writeAllocate == "no-write-allocate") {
        checkIfWriteAllocate = false;
    } else {
        std::cerr << "Error: write allocate command is invalid." << std::endl;
        exit(EXIT_FAILURE);
    }

    bool checkIfWriteThrough;
    if (write == "write-through") {
        checkIfWriteThrough = true;
    } else if (write == "write-back") {
        checkIfWriteThrough = false;
    } else {
        std::cerr << "Error: write through command is invalid." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (eviction != "lru" && eviction != "fifo") {
        std::cerr << "Error: eviction method command is invalid." << std::endl;
        exit(EXIT_FAILURE);
    }

    // check if write allocate is combined with write back (can't do this)
    if (!checkIfWriteAllocate && !checkIfWriteThrough) {
        std::cerr << "Error: no-write-allocate cannot be done with write-back."
                  << std::endl;
        exit(EXIT_FAILURE);
    }

    // create Cache obj
    Cache cache(numberOfSets, blocksPerSet, bytesPerBlock, writeAllocate, write, eviction);

    // read trace file (simulate cache)
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }

        // parse the line
        char accessType;
        unsigned int address;
        int size; // ignore for ms1

        if (sscanf(line.c_str(), "%c 0x%x %d", &accessType, &address, &size) != 3) {
            std::cerr << "Error: invalid command line: " << line << std::endl;
            continue;
        }
        cache.accessCache(std::string(1, accessType), address);
    }

    // output the required stats (using placeholders for now - ms1)
    std::cout << "Total loads: " << cache.totalLoads << std::endl;
    std::cout << "Total stores: " << cache.totalStores << std::endl;
    std::cout << "Load hits: " << cache.loadHits << std::endl;
    std::cout << "Load misses: " << cache.loadMisses << std::endl;
    std::cout << "Store hits: " << cache.storeHits << std::endl;
    std::cout << "Store misses: " << cache.storeMisses << std::endl;
    std::cout << "Total cycles: " << cache.totalCycles << std::endl;

    return 0;
}
