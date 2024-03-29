/*
CS342 Project 3
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 16.04.2019
*/
#ifndef RALLOC_H
#define RALLOC_H

#include <pthread.h>

#define MAX_RESOURCE_TYPES 10 
#define MAX_PROCESSES 20 

#define DEADLOCK_NOTHING   1
#define DEADLOCK_DETECTION 2
#define DEADLOCK_AVOIDANCE 3

int ralloc_init(int p_count, int r_count, int r_exist[], int d_handling); 
int ralloc_maxdemand(int pid, int r_max[]);
int ralloc_request (int pid, int demand[]); 
int ralloc_release (int pid, int demand[]);
int ralloc_detection(int procarray[]);
int ralloc_end(); 

#endif /* RALLOC_H */