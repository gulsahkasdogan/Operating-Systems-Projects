/*
CS342 Project 2
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 29.03.2019
*/
#include "hash.h" //check again
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include "pthread.h"

int N = 1000;
int K = 10;
int W = 10000;
int T = 5;
HashTable* hashtable;
pthread_mutex_t lock;

void* test_func(void* input){
	pthread_mutex_lock(&lock);

	void* value;
	int* val;
	
	for(int i = 0; i< W; i++){
		val = (int*)malloc(sizeof(int));
		(*val) = i;
		value = (void*)val;
		hash_insert(hashtable, i ,value);
	}

	/*update time*/
	for(int i = 0; i< W; i++){
		val = (int*)malloc(sizeof(int));
		(*val) = i + 200;
		value = (void*)val;
		hash_update(hashtable, i ,value);
	}

	/*get time*/
	for(int i = 0; i< W; i++){
		hash_get(hashtable, i, &value);
	}


	/*delete time*/
	for(int i = 0; i< W; i++){
		hash_delete(hashtable, i);
	}

	pthread_mutex_unlock(&lock);
	return NULL;
}

int main(){
	struct timeval t1,t2;
	double test_time;
	hashtable = hash_init(N, K);
	void* args = (void*)&W; 
	 
	pthread_t threads[T];
	for (int i = 1; i < 11 ; ++i)
	{
		K = i * 10;
		gettimeofday(&t1, NULL);
		for (int i = 0; i < T; ++i)
		{
			pthread_create(&(threads[i]), NULL, test_func, args);
		}

		for (int i = 0; i < T; ++i)
		{
			pthread_join(threads[i], NULL);
		}

		gettimeofday(&t2, NULL);

		test_time = (t2.tv_sec - t1.tv_sec) * 1000.0;
		test_time += (t2.tv_usec - t1.tv_usec) / 1000.0; //time in milliseconds
		printf("K: %d time: %lf\n" ,K, test_time);
	}
	

return 0;
}