##### Demo of MALLOC ARENAS:
    This test runs 5 iterations and in each iteration 100 threads are created 
    which mallocs 100mb each and frees the memory too, so the peak memory would
    be around 10g in each iteration.
    In total, memory allocated was 50g, and peak thread count is 500.
    
##### Tests results with top command (linux, 48 core, 130g):
```
   With default arenas 384 (#cores * 8):
	  VIRT memory: peak-28g retained-28g (and its not re-claimed even if all threads are ended)
	  RES memory : peak-4g retained-4g (and its not re-claimed even if all threads are ended)
 
  With MALLOC_ARENA_MAX = 2
 	  VIRT memory: peak-5.5g retained-4g (on all threads ended)
	  RES memory : peak-1.3g retained-700mb (on all threads endef)
```

##### Conclusion:
    Malloc arenas gives higher concurrency at memory allocation level, which helps multi-threaded applications
    with better throughput and less blocking at memory alloc/free operations.
    But, it also may consume higher memory footprint which gets shown in various statistics.
    Also kernel may not reclaim the freed memory from multiple arenas till there is a real need of memory by
    kernel for other requests, so memory freeing may be delayed till it is absolutely needed.
    A good practice maybe to keep MALLOC_ARENA_MAX = <number of cores>
    
##### ref: https://sourceware.org/glibc/wiki/MallocInternals 
