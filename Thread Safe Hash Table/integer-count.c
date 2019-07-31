/*
CS342 Project 2
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 29.03.2019
*/
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "hash.h"

#define MAX_FILE_LENGTH 500

int N = 1000;
int K = 100;
HashTable *ht1; // space allocated inside library
pthread_mutex_t lock;

void* counter(void* file){
	FILE *fp;
	char arr[MAX_FILE_LENGTH];
	char* file_name = (char*)file;
	
	pthread_mutex_lock(&lock);
	fp = fopen(file_name, "r");
	//lock
	if (fp == NULL)
	{
		printf("File can't be opened.\n");
		//unlock 
		pthread_mutex_unlock(&lock);
		return NULL;
	}

	while(fgets(arr, MAX_FILE_LENGTH, fp) != NULL){
		void* vp = (void*)malloc(sizeof(int));
		int key = atoi(arr);
		int success = hash_get(ht1,key, &vp);
		
		if(success == -1){
			int* first_val = (int* )malloc(sizeof(int));
			(*first_val) = 1;
			void* first_value = (void*)(first_val);
			hash_insert(ht1,key,first_value);
		}
		else{
			int* new_ptr = (int*)malloc(sizeof(int));
			(*new_ptr) = (*(int*)vp) + 1;
			hash_update(ht1, key,(void*)new_ptr);
		} 
	}

	//unlock 
	fclose(fp);
	pthread_mutex_unlock(&lock);
	return NULL;
}

int main(int argc, char **argv)
{

	FILE* out_fd;
	char* out_filename =  argv[argc-1];

	out_fd = fopen(out_filename, "w");

	dup2(fileno(out_fd), 1);

    ht1 = hash_init(N, K);
    int n = argc - 2;
    pthread_t threads[n];
   	if(argc > 2){
    	for (int i = 2; i < argc ; ++i)
    	{
    		void* counter_ptr = (void*)argv[i];
    		pthread_create(&(threads[i-2]),NULL, counter, counter_ptr);
    	}
    	
    }

    //THREAD JOIN
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    //write result to output file 
  	sort_display(ht1);

    hash_destroy(ht1);
}