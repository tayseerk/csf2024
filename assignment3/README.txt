**Assignment #3: Cache Simulator**

--------------------

*MS1*
Tayseer Karrossi: Makefile, main.cpp, cache.cpp
Emily Zou: cache.h

--------------------

*MS2*
Tayseer Karrossi:

- fixed implementation for load, loadToCache, store, storeToCache, getFreeIndex, emptySlot, 
  findBlock, findSetIndex, findTag, findBlockWithinSet, chooseBlockToEvict, and 
  findLeastRecentlyUsed
- implemented accessCache and updateLRU

Emily Zou:

- started implementation for load, loadToCache, store, storeToCache, getFreeIndex, emptySlot, 
  findBlock, findSetIndex, findTag, findBlockWithinSet, chooseBlockToEvict, and 
  findLeastRecentlyUsed

--------------------

*MS3*
Tayseer Karrossi:

- modified implementation for loadToCache, storeToCache, load, store, and chooseBlockToEvict
- implemented findFirstBlockIn

Emily Zou: Read Me - Best Cache Write-Up

Best Cache Write-Up:
For this assignment, we will be considering the "best cache" as the cache with the least load/store misses, the most load/store hits, and the least total cycles. 
A cache with the most hits and least misses indicates that the CPU was able to find the data it requested easily in the cache and is not slowed down by the need to fetch data from the main memory.
Therefore, an efficient cache will also have a lower number of total cycles since it is able to requests quickly. The tests that we made include a variety of direct mapped caches,
set associative caches, and fully associative caches. We then testes simple caches of each, then caches at an intermediate level of complexity, and caches with a lot more complexity. Testing many
levels of complexity ensures that we can take the caches into full account of every context that they may be used. We tested these caches in both gcc.trace files and swim.trace file for a variety of results,
also to ensure that results aren't due to a specific component of one file and don't contradict each other in different files. 

From the tests, it is clear that set associative is the most efficient cache according to our simulator. As we can see, for all of the traces, a set associative cache always has the most load/store hits, 
least load/store misses, and the least total cycles. From the gcc.trace file, the set associative cache was more efficient all throughout the different levels of complexity. It was evident in the beginning,
but the gap between the loads and misses of the set associative becomes greater as the complexity increases. In the most complicated cache tests, set associative only has about 78% less load misses than direct map does
and 48% less load misses than the fully associative cache. For the swim.trace file, set associative is also always the most efficient cache, but the difference in efficiency is greates in the intermediate level test.
In this test, direct map load hits 72% less times than set associative and fully associative cache load hits about 27% times less. It is also evident in the complicated cache configuration test with set associative cache
only having 451 load misses and direct map and dully associative cache having 2627 and 1094 load misses, respectively. More detailed test results are below.

Our test results are consistent with the nature of how each cache works. Direct maps were the least efficient cache, frequently being the cache with the most misses and least hits, since each block is mapped to 
exactly one cache set. Although this allows for low access time, because each block can only be mapped to exactly one place, more misses occursince blocks must ecivt each other more frequently when the cache is full, 
leading to more and more cache misses. Fully associative caches are also less efficient, but usually more efficient than direct maps. In a fully associative map, a cache can be placed anywhere, so it causes less conflict misses than
a direct map would. They can also locate caches better since they are free to place data in an order of most recently/frequently accessed. However, the more complex the cache becomes, the more expensive it becomes to locate
a block in the cache. Since they do not have an order for placing the blocks, the bigger the cache becomes, the longer it will take to find the block. However, set associative caches are the most efficient because they
combine the best 0parts of the other two caches. Set associative caches divide the cache into several sets with multiple blocks. A block is then mapped to anywhere in the specific set.This reduces misses because there is more options
for where a block can be placed, but still associating it with a specific set to reduce the time it takes to search for the block the bigger the cache becomes. Our tests reflect these behaviors.



Tests:

Tests Results (gcc.trace):
  Simple Direct Map: ./csim 1 1 4 write-allocate write-through lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 13415
    Load misses: 304782
    Store hits: 13556
    Store misses: 183930
    Total cycles: 69135483
  simple set associative: ./csim 2 1 4 write-allocate write-through lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 20570 - hits a lot more than direct map and fully associative
    Load misses: 297627 - misses less 
    Store hits: 14457 - more hits
    Store misses: 183029 - less misses
    Total cycles: 68329883 - less cycles
  simple fully associative: ./csim 1 1 4 write-allocate write-through fifo < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 13415
    Load misses: 304782
    Store hits: 13556
    Store misses: 183930
    Total cycles: 69135483
  intermediate direct map: ./csim 16 1 8 write-allocate write-through lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 153483 - a lot less hits (worse)
    Load misses: 164714 - a lot more misses (worse)
    Store hits: 110043
    Store misses: 87443 - most misses (worse)
    Total cycles: 70695683
  intermediate set associative: ./csim 16 4 16 write-allocate write-through lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 297243 - most load hits
    Load misses: 20954 - least misses by a lot
    Store hits: 184121 - most hits
    Store misses: 13365 - least misses by a lot
    Total cycles: 33991883 - least cycles by more than half
  intermediate fully associative: ./csim 1 16 16 write-allocate write-through fifo < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 229708
    Load misses: 88489
    Store hits: 143148
    Store misses: 54338
    Total cycles: 77395083
  complicated direct map: ./csim 256 1 64 write-allocate write-through lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 312297
    Load misses: 5900 - most load misses by a lot (worse)
    Store hits: 194064
    Store misses: 3422
    Total cycles: 35179483
  complicated set associative: ./csim 256 4 64 write-allocate write-through lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 316955 - most hits
    Load misses: 1242 - least load misses
    Store hits: 195056 - most hits
    Store misses: 2430 - least misses
    Total cycles: 26139483 - least cycles
  complicated fully associative: ./csim 1 256 64 write-allocate write-through fifo < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 315807
    Load misses: 2390
    Store hits: 194763
    Store misses: 2723
    Total cycles: 28445083
  
Test Results (swim.trace)
  Simple Direct Map: ./csim 1 1 4 write-allocate write-through lru < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 16869
    Load misses: 203799
    Store hits: 6150
    Store misses: 76375
    Total cycles: 36573093
  simple set associative: ./csim 2 1 4 write-allocate write-through lru < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 17392 - most hits 
    Load misses: 203276 - least misses 
    Store hits: 7989 - most hits
    Store misses: 74536 - least misses
    Total cycles: 36336893 - least cycles
  simple fully associative: ./csim 1 1 4 write-allocate write-through fifo < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 16869
    Load misses: 203799
    Store hits: 6150
    Store misses: 76375
    Total cycles: 36573093
  intermediate direct map: ./csim 16 1 8 write-allocate write-through lru < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 57269
    Load misses: 163399
    Store hits: 40338
    Store misses: 42187
    Total cycles: 49672893
  intermediate set associative: ./csim 16 4 16 write-allocate write-through lru < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 206490 - most hits by a lot (4x direct map and almost 2x fully associative)
    Load misses: 14178 - least misses by a lot (again, almost 4x less than direct map)
    Store hits: 68489 - most
    Store misses: 14036 - least by a lot 
    Total cycles: 19841293 - least (almost half of the other two)
  intermediate fully associative: ./csim 1 16 16 write-allocate write-through fifo < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 149251
    Load misses: 71417
    Store hits: 58259
    Store misses: 24266
    Total cycles: 46828893
  complicated direct map: ./csim 256 1 64 write-allocate write-through lru < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 218041
    Load misses: 2627
    Store hits: 78826
    Store misses: 3699
    Total cycles: 18677293
  complicated set associative: ./csim 256 4 64 write-allocate write-through lru < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 220217 - most
    Load misses: 451 - least by a lot
    Store hits: 79454 - most
    Store misses: 3071 - least
    Total cycles: 14190893
  complicated fully associative: ./csim 1 256 64 write-allocate write-through fifo < swim.trace
    Total loads: 220668
    Total stores: 82525
    Load hits: 219574
    Load misses: 1094
    Store hits: 79310
    Store misses: 3215
    Total cycles: 15450093

