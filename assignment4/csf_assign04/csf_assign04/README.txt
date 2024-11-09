**CONTRIBUTIONS**

Emily Zou:
- completed Task 1, Task 2, and Task 3

Tayseer Karrossi:
- fixed Task 3 and completed Task 4

**REPORT**

*Experiment Details*

I'm running this experiment using 8 GB RAM of memory and 4 CPU Cores (Intel Core i5). 
The file size for this experiment will be 16 MB, so 2097152 elements in total (8 bytes for int64_t).
The experiment will be run 5 times, and the average of all the real/user/sys times will be used for reference when analyzing. 
The threshold values that will be tested in this experiment are:
- 2097152
- 1048576
- 524288
- 262144
- 131072
- 65536
- 32768
- 16384

*Results*

---- Trial 1 ---- 

Test run with threshold 2097152

real    0m0.928s
user    0m0.875s
sys     0m0.041s
Test run with threshold 1048576

real    0m0.408s
user    0m0.664s
sys     0m0.074s
Test run with threshold 524288

real    0m0.283s
user    0m0.675s
sys     0m0.078s
Test run with threshold 262144

real    0m0.231s
user    0m0.681s
sys     0m0.119s
Test run with threshold 131072

real    0m0.206s
user    0m0.672s
sys     0m0.128s
Test run with threshold 65536

real    0m0.197s
user    0m0.693s
sys     0m0.197s
Test run with threshold 32768

real    0m0.210s
user    0m0.823s
sys     0m0.309s
Test run with threshold 16384

real    0m0.203s
user    0m0.778s
sys     0m0.456s

---- Trial 2 ---- 

Test run with threshold 2097152

real    0m0.970s
user    0m0.915s
sys     0m0.042s
Test run with threshold 1048576

real    0m0.423s
user    0m0.697s
sys     0m0.073s
Test run with threshold 524288

real    0m0.298s
user    0m0.716s
sys     0m0.092s
Test run with threshold 262144

real    0m0.249s
user    0m0.707s
sys     0m0.118s
Test run with threshold 131072

real    0m0.213s
user    0m0.692s
sys     0m0.152s
Test run with threshold 65536

real    0m0.213s
user    0m0.735s
sys     0m0.182s
Test run with threshold 32768

real    0m0.216s
user    0m0.743s
sys     0m0.287s
Test run with threshold 16384

real    0m0.213s
user    0m0.799s
sys     0m0.513s

---- Trial 3 ---- 

Test run with threshold 2097152

real    0m0.758s
user    0m0.717s
sys     0m0.034s
Test run with threshold 1048576

real    0m0.415s
user    0m0.748s
sys     0m0.075s
Test run with threshold 524288

real    0m0.302s
user    0m0.733s
sys     0m0.091s
Test run with threshold 262144

real    0m0.238s
user    0m0.704s
sys     0m0.108s
Test run with threshold 131072

real    0m0.205s
user    0m0.680s
sys     0m0.158s
Test run with threshold 65536

real    0m0.195s
user    0m0.706s
sys     0m0.182s
Test run with threshold 32768

real    0m0.209s
user    0m0.756s
sys     0m0.315s
Test run with threshold 16384

real    0m0.209s
user    0m0.766s
sys     0m0.485s

---- Trial 4 ---- 

Test run with threshold 2097152

real    0m0.723s
user    0m0.686s
sys     0m0.028s
Test run with threshold 1048576

real    0m0.416s
user    0m0.663s
sys     0m0.083s
Test run with threshold 524288

real    0m0.289s
user    0m0.673s
sys     0m0.095s
Test run with threshold 262144

real    0m0.232s
user    0m0.689s
sys     0m0.113s
Test run with threshold 131072

real    0m0.208s
user    0m0.670s
sys     0m0.147s
Test run with threshold 65536

real    0m0.202s
user    0m0.684s
sys     0m0.203s
Test run with threshold 32768

real    0m0.204s
user    0m0.717s
sys     0m0.286s
Test run with threshold 16384

real    0m0.201s
user    0m0.768s
sys     0m0.466s

---- Trial 5 ---- 

Test run with threshold 2097152

real    0m0.800s
user    0m0.751s
sys     0m0.040s
Test run with threshold 1048576

real    0m0.550s
user    0m0.793s
sys     0m0.081s
Test run with threshold 524288

real    0m0.291s
user    0m0.675s
sys     0m0.100s
Test run with threshold 262144

real    0m0.262s
user    0m0.716s
sys     0m0.110s
Test run with threshold 131072

real    0m0.212s
user    0m0.692s
sys     0m0.134s
Test run with threshold 65536

real    0m0.197s
user    0m0.697s
sys     0m0.179s
Test run with threshold 32768

real    0m0.206s
user    0m0.724s
sys     0m0.307s
Test run with threshold 16384

real    0m0.201s
user    0m0.771s
sys     0m0.444s

---- Averages ----

Test run with threshold 2097152

real  0m0.836s
user  0m0.789s
sys   0m0.037s

Test run with threshold 1048576

real  0m0.442s
user  0m0.713s
sys   0m0.077s

Test run with threshold 524288

real  0m0.293s
user  0m0.694s
sys   0m0.091s

Test run with threshold 262144

real  0m0.242s
user  0m0.699s
sys   0m0.114s

Test run with threshold 131072

real  0m0.209s
user  0m0.681s
sys   0m0.144s

Test run with threshold 65536

real  0m0.201s
user  0m0.703s
sys   0m0.189s

Test run with threshold 32768

real  0m0.209s
user  0m0.753s
sys   0m0.301s

Test run with threshold 16384

real  0m0.205s
user  0m0.776s
sys   0m0.473s

*Analysis*

The real time decreases as you decrease the parallel threshold since the avg real time for the threshold 2097152 is 0.836s where as for the threshold 131072, it is 0.209s which is significantly lower. The performance improvement is due to increased parallelism and better CPU utilization. Lowering the threshold lets the program create more child processes earlier during recursion, allowing more of the array to be sorted at the same time. With more child processes created, the program can also optimize its usage of the 4 CPU cores, which would reduce the total real time. 

The decrease in real time occurs gradually as you decrease the threshold until the improvement in performance plateaus once you reach 131072. Any threshold less than 131072 (inclusive) ranges from 0.201 to 0.209 seconds. The main reason that the improvements become minimal is the limited number of CPU cores. Once you create enough child processes, you use all of the cores available (4 of them) and by adding more processes, the real time doesn't improve any further. Another reason for the plateau could also be that the benefit of parallelism diminishes once the workload per child process is too small relative to the overhead. 

The average user time also decreases until it reaches the "plateau" threshold of 131072. Anything threshold lower than 131072 results in an increase in avg user time. This is represented in the data where the threshold of 2097152 has an avg user time of 0.789s and it decreases until you reach 131072 which has an avg user time of 0.681s. After that threshold, the avg user time starts to increase where the threshold of 16384 has an avg user time of 0.776s (almost as high as the 2097152 threshold). This occurs because of the same reason, the performance increase plateaus for the real time. Once you reach a threshold that creates enough child processes to utilize all 4 CPU cores, any additional child processes would add to the user time (increased CPU usage) since there are more recursive func calls and computations. 

The average system time increases as you decrease the threshold since the highest threshold 2097152 has an avg sys time of 0.037s and the lowest threshold 16384 has an avg system time of 0.473s. This is because as more processes are made, the time spent on system calls and kernel operations increases. As the number of child processes increases, the number of 'fork' calls increases. Additionally, with more child processes, the OS scheduler and memory have to handle more processes which adds to the system time. 

Based on all the results observed, the most optimal threshold for parsort would be 131072 because it balances the parallelism and overhead. At the threshold of 131072, the avg real and user time are at their lowest values before the plateau occurs. Additionally, it also has a good avg sys time since it is less than half of our range. The highest sys time is for the lowest threshold at 0.473s and the lowest sys time is for the highest threshold at 0.037s. 0.473s minus 0.037s = 0.436s / 2 = 0.218s. The avg sys time for the optimal threshold is 0.144s which is less than 0.218s.










