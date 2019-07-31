#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "ralloc.c"

int handling_method;          // deadlock handling method

#define M 3                   // number of resource types
int exist[M] =  {12, 8, 10};  // resources existing in the system

#define N 5                 // number of processes - threads
pthread_t tids[N];            // ids of created threads
int flag = 0; 
void *aprocess (void *p)
{
    int req[M];
    int k;
    int pid;
    int deadlocked[N];

    pid =  *((int *)p);
    
    printf ("this is thread %d\n", pid);
    fflush (stdout);

        //process 1 requests (1,0,2)
    if(pid == 1){
        req[0] = 0;
        req[1] = 2;
        req[2] = 0;
        ralloc_request(pid, req);
        ralloc_release(pid, req);
        flag = 1;
    }

    if(pid == 0){
        req[0] = 1;
        req[1] = 0;
        req[2] = 2;
        ralloc_request(pid, req);
        ralloc_detection(deadlocked);
        //ralloc_release(pid, req);
    }
    
    pthread_exit(NULL); 
}


int main(int argc, char **argv)
{
    int dn;
    int pids[N];
    int i;
    int deadlocked[N];
    handling_method = DEADLOCK_DETECTION;
    ralloc_init (N, M, exist, handling_method);
    printf ("library initialized\n");

    int available[M] = {3,3,2};
    int alloc[N][M] = {{2,0,0},{0,1,0}, {3,0,2}, {2,1,1}, {0,0,2}};
    int max[N][M] = {{3,2,2},{7,5,3},{9,0,2}, {2,2,2}, {4,3,3}};

    set_all(alloc, max, available);
    //display_available();
    //display_max();
    //display_allocation();

    //ralloc_detection(deadlocked);
    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }
    
    fflush (stdout);
    for(int i = 0; i<N; i++){
        pthread_join(tids[i], NULL);
    }
    ralloc_end();

    printf ("threads created = %d\n", N);
}