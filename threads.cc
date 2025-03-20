// build: $ g++ -pthread -o threads threads.cc
// run:   $ ./threads

/**
 Demo of MALLOC ARENAS:
    This test runs 5 iterations and in each iteration 100 threads are created 
    which mallocs 100mb each and frees the memory too, so the peak memory would
    be around 10g in each iteration.
    In total, memory allocated was 50g, and peak thread count is 500.
    
 Tests results with top command (linux, 48 core, 130g):
 With default arenas 384 (#cores * 8):
	VIRT memory: peak-28g retained-28g (and its not re-claimed even if all threads are ended)
	RES memory : peak-4g retained-4g (and its not re-claimed even if all threads are ended)

 With MALLOC_ARENA_MAX = 2
 	VIRT memory: peak-5.5g retained-4g (on all threads ended)
	RES memory : peak-1.3g retained-700mb (on all threads endef)
	
 Conclusion:
    Malloc arenas gives higher concurrency at memory allocation level, which helps multi-threaded applications
    with better throughput and less blocking at memory alloc/free operations.
    But, it also may consume higher memory footprint which gets shown in various statistics.
    Also kernel may not reclaim the freed memory from multiple arenas till there is a real need of memory by
    kernel for other requests, so memory freeing may be delayed till it is absolutely needed.
    A good practice maybe to keep MALLOC_ARENA_MAX = <number of cores>
    
 ref: https://sourceware.org/glibc/wiki/MallocInternals 

*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <map>
#include <time.h>

using namespace std;

volatile int kill = 0;

void* threadFunction(void* arg) {
     // 100 mb heap total
    int mb1 = 1 * 1024 * 1024;
    int mb10 = mb1 * 10;
    int i = 0;
    int  blocks = 4;
    void* ptr[100];
    int c = 0;

        for (; i < 50; ++i) {
                char *p = (char *)malloc(mb1);
                memset((char *)p, 1, mb1);
                sleep(1);
                ptr[c++] = p;
                free(p);
        }
        i = 0;
        for (; i < 5; ++i) {
                char *p = (char *)malloc(mb10);
                memset((char *)p, 1, mb10);
                sleep(1);
                ptr[c++] = p;
                free(p);
        }

    // some local stl
    map<string, string> m[100];
    m[0].insert({"meranaamjoker", "meranaamjoker"});
    m[1].insert({"teranaamjoker", "teranaamjoker"});

    sleep(10);

     // wait for kill signal
    while (kill != 1)
       sleep(1);

    return NULL;
}

void testThreads(int n) {
    pthread_t threads[n];
    int result = 0;
    pthread_t id;
    int i = 0;

     for (; i < n; ++i) {
          int s =  (i % 2) == 0;
          pthread_create(&id, NULL, threadFunction, &s);
          threads[i] = id;
     }

    cout << "threads created : " << n << endl;
    sleep(10);

    cout << " done" << endl;
}

int main() {
    int i = 1;
    int iter = 5;
    int threadCount = 100;
    cout << " PID: " << getpid() << endl;

    cout << "  ------------------ " << endl;
    malloc_stats();

    time_t start,end;
    time (&start);

    for (; i <= iter; ++i) {
        testThreads(threadCount);
        sleep(5);
        cout << "  ------------------  " << endl;
        malloc_stats();
    }

    cout << " PID: " << getpid() << endl;

    time (&end);
    double dif = difftime (end,start);
    cout << "Time taken is seconds = " <<  dif << endl;

    char ch;
    cout << " finish threads... ";
    cin >> ch;
    kill = 1;

    sleep(30);

    cout << "  ------------------ " << endl;
    malloc_stats();

    cout << "DONE" << endl;
    return 0;
}

