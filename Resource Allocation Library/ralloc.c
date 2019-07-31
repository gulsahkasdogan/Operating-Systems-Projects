/*
CS342 Project 3
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 16.04.2019
*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "ralloc.h"
#include <assert.h>

int hand_type; // 1: DEADLOCK_NOTHING, 2: DEADLOCK_DETECTION, 3: DEADLOCK_AVOIDANCE
int** allocation;
int** max_alloc;// will be used in avo
int* available;
int* total;
int resource_count;
int process_count;
int temp_pid;

int process_num; //NEW
int result[20]; //NEW 20 yerine process_count
int k; //NEW 

int deadlocks[20];

pthread_mutex_t lock, lock_detect;
pthread_cond_t request_c = PTHREAD_COND_INITIALIZER;

/* Helper functions */
void display_allocation(){
    int* curr;
    if(allocation != NULL){
        printf("ALLOCATION:\n");
        for(int i = 0; i < process_count; i++){
            curr = allocation[i];
            if(curr != NULL){
                printf("pid %d: ",i);
                for(int j = 0; j < resource_count; j++){
                    printf(" %d", curr[j]);
                }
                printf("\n");
            }
        }
    }
}

void display_max(){
    int* curr;
    if(max_alloc != NULL){
        printf("MAX:\n");
        for(int i = 0; i < process_count; i++){
            curr = max_alloc[i];
            if(curr != NULL){
                printf("pid %d: ",i);
                for(int j = 0; j < resource_count; j++){
                    printf(" %d", curr[j]);
                }
                printf("\n");
            }
        }
    }
}

void display_available(){
    printf("AVAILABLE: ");
    if(available != NULL){
        for (int i = 0; i < resource_count; ++i)
        {
            printf(" %d", available[i]);
        }
        printf("\n");
    }
}

void set_all(int alloc[process_count][resource_count],
            int max[process_count][resource_count], 
            int avail[resource_count]){

    for(int i = 0; i < process_count; i++){
        for(int j = 0; j  < resource_count; j++){
            allocation[i][j] = alloc[i][j];
            max_alloc[i][j] = max[i][j];
        }
    }
    for(int i = 0; i<resource_count; i++){
        available[i] = avail[i];
    }
}


/* Library functions */
int ralloc_init(int p_count, int r_count, int r_exist[], int d_handling)
{
    resource_count = r_count;
    process_count = p_count;
    hand_type = d_handling;

    allocation = (int**)malloc(p_count * sizeof(int*));
    max_alloc = (int**)malloc(p_count * sizeof(int*));
    available = (int*) malloc(r_count * sizeof(int));
    total = (int*) malloc(r_count * sizeof(int));

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock_detect, NULL);
    //initially all resources are available(max)
    for (int i = 0; i < r_count; ++i)
    {
        available[i] = r_exist[i];
        total[i] = r_exist[i]; //record the total number of resources
    }

    //allocate space for allocation array
    for (int i = 0; i < p_count; ++i)
    {
        allocation[i] = (int*)malloc(r_count * sizeof(int));
        for(int j = 0; j < r_count; j++){
            allocation[i][j] = 0;
        }
    }

    //allocate space for max array
    for (int i = 0; i < p_count; ++i)
    {
        max_alloc[i] = (int*)malloc(r_count * sizeof(int));
    }
    for (int i = 0; i < process_count; ++i)
    {
        deadlocks[i] = 0;
    }
    return (0);
}

int ralloc_maxdemand(int pid, int r_max[]){
    pthread_mutex_lock(&lock);

    for (int i = 0; i < resource_count; ++i)
    {
        max_alloc[pid][i] = r_max[i];
    }
    pthread_mutex_unlock(&lock);
    return (0);
}

int compare_aval_to_demand(int demand[]){
    int result = 1;
    for (int i = 0; i < resource_count; ++i)
    {
        if(available[i] < demand[i]){
            result = 0;
            break;
        }
    } 
    return result;   
}

int compare_max_to_demand(int pid, int demand[]){
    int result = 1;
    for (int i = 0; i < resource_count; ++i)
    {
        if(max_alloc[pid][i] < demand[i]){
            result = 0;
            break;
        }
    }  
    return result;  
}

int compare_total_to_demand(int demand[]){
    int result = 1;
    for (int i = 0; i < resource_count; ++i)
    {
        if(total[i] < demand[i]){
            result = 0;
            break;
        }
    }    
    return result;
}


int avoidance_helper(int pid, int demand[]){
    //avoidance
    int less_than_max = compare_max_to_demand(pid, demand);
    int less_than_total = compare_total_to_demand(demand);
    int less_than_available = compare_aval_to_demand(demand);

    int copy_alloc[process_count][resource_count];
    int copy_avail[process_count];

    for(int i = 0; i < process_count; i++){
        for(int j = 0; j < resource_count; j++){
            copy_alloc[i][j] = allocation[i][j];
        }
    }

    for(int i = 0; i < resource_count; i++){
        copy_avail[i] = available[i];
    }

    if(less_than_max && less_than_available && less_than_total){
        for(int i = 0; i < resource_count; i++){
            copy_avail[i] -= demand[i];
            copy_alloc[pid][i] += demand[i];
        }
        int need[process_count][resource_count];
        int finish[process_count];
        int work[process_count];  //NEW

        for (int n = 0; n < process_count; ++n)
        {
            finish[n] = 0;
        }
        
        for (int m = 0; m < resource_count; ++m)
        {
            work[m] = copy_avail[m]; //changed-gulsah
        }
       
        for (int i = 0; i < process_count; ++i)
        {
            for (int j = 0; j < resource_count; ++j)
            {
                need[i][j] = max_alloc[i][j] - copy_alloc[i][j];
            }
        }
    
        //check if safe 
        int need_less_than_work;
        int found = 1;
        while(found == 1 ){
            int flag = 0;
            for(int i = 0; i < process_count; i++){
                found = 0;
                //compare need_i and work
                need_less_than_work = 1;
                for(int j = 0; j<resource_count; j++){
                    if(need[i][j] > work[j]){
                        need_less_than_work = 0;
                        break;
                    }
                }
                //check if finish[i] == false and need_i < work
                if(finish[i] == 0 && need_less_than_work == 1){
                    //found one process 
                    found = 1; //as long as there exists such process 
                    finish[i] = 1; 
                    flag = 1;
                    //work = work + allocation_i
                    for(int j = 0; j < resource_count; j++){
                        work[j] += copy_alloc[i][j];
                    }
                    //break the for loop, start again 
                    break;
                }
            }   
            if(flag == 0)
                break;
        }
        int safe = 1;
        for(int i = 0; i<process_count; i++){
            if(finish[i] == 0){
                safe = 0;
                break;
            }
        }

        if(safe){
            //safe
            return 1;
        }
        else{
            //deadlock might occur
            return 0;
        }
    }
    return 0;
}


int ralloc_request (int pid, int demand[]){
    pthread_mutex_lock(&lock);
    //int dead[process_count];
    temp_pid = pid;
    int less_than_total = compare_total_to_demand(demand);
    int less_than_available = compare_aval_to_demand(demand);
    if(hand_type != 3){
        //do nothing
        if(less_than_total == 1){
            while(less_than_available == 0){
                //sleep
                pthread_cond_wait(&request_c, &lock);
                less_than_available = compare_aval_to_demand(demand);
            }
            //return from wait
            //ver --> less_than_available == 1
            for (int i = 0; i < resource_count; ++i)
            {
                allocation[pid][i] += demand[i];
                available[i] -= demand[i];
            }     
        }
        else{
            //unsuccessful request 
            pthread_mutex_unlock(&lock);
            return -1;
        }

    }

    else if(hand_type == 3){
        while(avoidance_helper(pid, demand) != 1){
            pthread_cond_wait(&request_c, &lock);
        }
        //allocation yapılabilir
        for(int i = 0; i < resource_count; i++){
            available[i] -= demand[i];
            allocation[pid][i] += demand[i];
        }   
    }  
    pthread_mutex_unlock(&lock);
    return 0;
}

int ralloc_release (int pid, int demand[]) {
    pthread_mutex_lock(&lock);
    if(hand_type == 1){ 
        //do nothing
        for(int i = 0; i<resource_count; i++){
            allocation[pid][i] = allocation[pid][i] - demand[i];
            available[i] += demand[i];
        }
        pthread_cond_broadcast(&request_c);

    }else if(hand_type == 2){
        //detection
        for(int i = 0; i<resource_count; i++){
            allocation[pid][i] = allocation[pid][i] - demand[i];
            available[i] += demand[i];
        }
        pthread_cond_broadcast(&request_c);
    }else if(hand_type == 3){
        for(int i = 0; i<resource_count; i++){
            allocation[pid][i] = allocation[pid][i] - demand[i];
            available[i] += demand[i];
        }
        pthread_cond_broadcast(&request_c);
    }
    pthread_mutex_unlock(&lock);
    return (0);
}

int ralloc_detection(int procarray[]) {
    pthread_mutex_lock(&lock_detect);
    int need[process_count][resource_count];
    int finish[process_count];
    int work[process_count];
    //set finish and work

    for (int n = 0; n < process_count; ++n)
    {
        finish[n] = 0; //false
    }
   
    for (int m = 0; m < resource_count; ++m)
    {
        work[m] = available[m]; 
    }

    for (int i = 0; i < process_count; ++i)
    {
        for (int j = 0; j < resource_count; ++j)
        {
            need[i][j] = max_alloc[i][j] - allocation[i][j];
        }
    }
    int need_less_than_work;
    int found = 1;
    while(found == 1 ){
        int flag = 0;
        for(int i = 0; i < process_count; i++){
            found = 0;
            //compare need_i and work
            need_less_than_work = 1;
            for(int j = 0; j<resource_count; j++){
                if(need[i][j] > work[j]){
                    need_less_than_work = 0;
                    break;
                }
            }
            //check if finish[i] == false and need_i < work
            if(finish[i] == 0 && need_less_than_work == 1){
                //found one process 
                found = 1; //as long as there exists such process 
                finish[i] = 1; 
                flag = 1;
                //work = work + allocation_i
                for(int j = 0; j < resource_count; j++){
                    work[j] += allocation[i][j];
                }
                //break the for loop, start again 
                break;
            }
        }   
        if(flag == 0)
            break;
    }

    //set return value
    int dead_count = 0;
    for (int a = 0; a < process_count; ++a)
    {
        if (finish[a] == 0)
        {
            dead_count++;
            procarray[a] = 1;
        }
        else{
            procarray[a] = -1;
        }
    }
    pthread_mutex_unlock(&lock_detect);
    return dead_count;
}

int ralloc_end() {
    pthread_mutex_lock(&lock);

    for (int i = 0; i < process_count; ++i)
    {
        if(allocation[i] != NULL)
            free(allocation[i]);
        if(max_alloc[i] != NULL)
            free(max_alloc[i]);
    }
    if(allocation != NULL)
        free(allocation);
    if(max_alloc != NULL)
        free(max_alloc);
    if(available != NULL)
        free(available);
    if(total != NULL)
        free(total);

    pthread_mutex_unlock(&lock);
    return (0);
}
