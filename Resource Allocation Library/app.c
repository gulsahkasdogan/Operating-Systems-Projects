/*
CS342 Project 3
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 16.04.2019
*/
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "ralloc.h"
#include <sys/time.h>
#include <time.h>

int handling_method;          // deadlock handling method

#define M 3                   // number of resource types
int exist[M] =  {12, 8, 10};  // resources existing in the system

#define N 15                   // number of processes - threads
pthread_t tids[N];            // ids of created threads
int detection_overhead_test = 0;


void *aprocess (void *p)
{
    int req[M];
    int k;
    int pid;
    int deadlocked[N]; // array indicating deadlocked processes

    for (k = 0; k < N; ++k)
        deadlocked[k] = -1; // initialize
    
    pid =  *((int *)p);
    
    printf ("this is thread %d\n", pid);
    fflush (stdout);
    
    req[0] = 4;
    req[1] = 4;
    req[2] = 4;
    ralloc_maxdemand(pid, req); 
    
    for (k = 0; k < 5; ++k) {

        if(pid == 0){
            req[0] = 0;
            req[1] = 0;
            req[2] = 1;  
        }
    
        // do something with the resources
        else{
            if(pid % 2 == 0){
            //pass
                req[0] = 1;
                req[1] = 0;
                req[2] = 0;
            }
            else{
                req[0] = 0;
                req[1] = 1;
                req[2] = 0;
            }
        }


        if(detection_overhead_test == 0){
            ralloc_request (pid, req);

            if(pid == 0){
                sleep(1);
            }

            sleep(2);        
            ralloc_release (pid, req);
        }
        else if(detection_overhead_test == 1){
            ralloc_request (pid, req);

            if(pid == 0){
                sleep(1);
            }

            sleep(2); 

            ralloc_detection(deadlocked);       
            ralloc_release (pid, req);
        }

        // call request and release as many times as you wish with
        // different parameters
    }
    pthread_exit(NULL); 
}


int main(int argc, char **argv)
{
    //int dn; // number of deadlocked processes
    //int k;
    int i = 0;
    int pids[N];
    struct timeval t1,t2;
    double test_time;

//AVOIDANCE
    handling_method = DEADLOCK_AVOIDANCE;
    ralloc_init (N, M, exist, handling_method);
    gettimeofday(&t1, NULL);

    //PROCESS
    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }

    for(int i = 0; i<N; i++){
        pthread_join(tids[i], NULL);
    }
    ralloc_end();

    gettimeofday(&t2, NULL);
    test_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
    test_time += (t2.tv_usec - t1.tv_usec) / 1000.0; //time in milliseconds
    printf("DEADLOCK_AVOIDANCE time: %lf\n" , test_time);


//NOTHING    
    handling_method = DEADLOCK_NOTHING;
    ralloc_init (N, M, exist, handling_method);
    gettimeofday(&t1, NULL);

    //PROCESS
    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }

    for(int i = 0; i<N; i++){
        pthread_join(tids[i], NULL);
    }
    ralloc_end();

    gettimeofday(&t2, NULL);
    test_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
    test_time += (t2.tv_usec - t1.tv_usec) / 1000.0; //time in milliseconds
    printf("DEADLOCK_NOTHING time: %lf\n" , test_time);



//DETECTION
    handling_method = DEADLOCK_DETECTION;
    ralloc_init (N, M, exist, handling_method);
    gettimeofday(&t1, NULL);

    //PROCESS
    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }

    for(int i = 0; i<N; i++){
        pthread_join(tids[i], NULL);
    }
    ralloc_end();

    gettimeofday(&t2, NULL);
    test_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
    test_time += (t2.tv_usec - t1.tv_usec) / 1000.0; //time in milliseconds
    printf("DEADLOCK_DETECTION WITHOUT DETECT time: %lf\n" , test_time);




//#2
    handling_method = DEADLOCK_DETECTION;
    ralloc_init (N, M, exist, handling_method);
    detection_overhead_test = 1; //we are now testing detection overhead 
    gettimeofday(&t1, NULL);

    //PROCESS
    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }    

    for(int i = 0; i<N; i++){
        pthread_join(tids[i], NULL);
    }
    ralloc_end();

    gettimeofday(&t2, NULL);
    test_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
    test_time += (t2.tv_usec - t1.tv_usec) / 1000.0; //time in milliseconds
    printf("DEADLOCK_DETECTION USING DETECT time: %lf\n" , test_time);


    exit(0);
}
