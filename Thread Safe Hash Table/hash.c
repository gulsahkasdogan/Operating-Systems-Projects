/*
CS342 Project 2
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 29.03.2019
*/
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t locks[100];

int simple_hash_function(int i, int N){
    int hash_val = 0;
    hash_val = i%N;
    return hash_val;
}

//tested
//return 1 if key exists
int keyExist(node* list, int k){
    node* curr = NULL;
    //check if key exists
    curr = list;
    while(curr != NULL){
        if(curr->key == k){
            //unlock
            return 1; //key exist

        }
        curr = curr->next;
    }
    return 0;
}

HashTable *hash_init (int N, int K){
    //N: number of buckets
    //K:number of locks
    //N/M = K
    HashTable* newTable = NULL;
    if(N < 1)
        return NULL;
    newTable = malloc(sizeof(HashTable));
    if(newTable == NULL){
        //memory allocation for the newTable itself
        return NULL;
    }
    if((newTable->table = malloc(sizeof(node*) * N)) == NULL){
        //memory allocation for N bucket's head pointers
        return NULL;
    }
    //If you survived until you reach here we can initialize the buckets as empty linked list
    newTable->size = N;
    newTable->K = K;
    for(int i = 0; i < N; i++){
        newTable->table[i] = NULL;
    }

    for (int i = 0; i < K; ++i)
    {
        pthread_mutex_init(&locks[i], NULL);
    }
    return newTable;
}

//tested
//return -1 if unsuccessful, 0 otherwise
int hash_insert (HashTable *hp, int k, void* v){
    int location; //location to insert
    node* target; //head of target
    node* newPair; //new pair to be inserted
    location = simple_hash_function(k, hp->size);
    //lock created & locked
    int M = hp->size / hp->K;
    int func_lock = location / M;
    pthread_mutex_lock(&locks[func_lock]);
    target = hp->table[location]; //add to here

    if(target == NULL){
        //add first item
        hp->table[location] = (struct key_value_pair*)malloc(sizeof(node));
        if( hp->table[location] != NULL){
            hp->table[location]->key = k;
            hp->table[location]->value = v;
            hp->table[location]->next = NULL;
        }
        //unlocked
        pthread_mutex_unlock(&locks[func_lock]);
        return 0;
    }

    else{
        if(keyExist(target, k) == 1){
            //unlocked
            pthread_mutex_unlock(&locks[func_lock]);
            return -1;
        }
        else{
            newPair = (struct key_value_pair*)malloc(sizeof(node));
            newPair->key = k;
            newPair->value = v;
            newPair->next = hp->table[location];
            hp->table[location] = newPair;
            //unlocked
            pthread_mutex_unlock(&locks[func_lock]);
            return 0;
        }
    }
}


int hash_delete (HashTable *hp, int k){
    if (hp == NULL){
        printf("%s\n", "The table does not exist.");
        return -1;
    }
    else{
        int location;
        node* curr;
        node* temp;
        location = simple_hash_function(k, hp->size);
        //create lock & locked
        int M = hp->size/hp->K;
        int func_lock = location / M;
        pthread_mutex_lock(&locks[func_lock]);

        curr = hp->table[location];
        
        if(curr != NULL && curr->key == k){
            //delete me
            hp->table[location] = curr->next;
            free(curr);
            pthread_mutex_unlock(&locks[func_lock]);
            return 0;
        }
        else{
            while(curr != NULL){
                if(curr->next->key == k){
                    //delete me
                    temp = curr->next;
                    curr->next = temp->next;
                    free(temp);
                    temp = NULL;
                    pthread_mutex_unlock(&locks[func_lock]);
                    return 0;
                }
                curr = curr->next;
            }
        }
        pthread_mutex_unlock(&locks[func_lock]);
        return -1;
    }
}

int hash_update (HashTable *hp, int k, void *v){
    int location;
    node* curr;
    location = simple_hash_function(k, hp->size); //bucket no of key k
    //create lock & locked
    int M = hp->size/hp->K;
    int func_lock = location / M;
    pthread_mutex_lock(&locks[func_lock]);
    //check if key exists
    curr =  hp->table[location];
    while(curr != NULL){
        if(curr->key == k){
            //update me
            curr->value = v;
            pthread_mutex_unlock(&locks[func_lock]);
            return 0;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&locks[func_lock]);
    return -1; //item does not exist of cannot update

}


int hash_destroy (HashTable *hp){
    node* curr = NULL;
    node* temp = NULL;
    //printf("a1\n");
    pthread_mutex_t hdestroy_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&hdestroy_lock);
    for(int i = 0; i<hp->size; i++){
        if(hp->table[i] != NULL){
            curr = hp->table[i];
            while(curr != NULL){
                //do something
                temp = curr;
                curr = curr->next;
                free(temp);
            }
        }
    }
    free(hp->table);
    free(hp);
    hp = NULL;
    pthread_mutex_unlock(&hdestroy_lock);
    return 0;
}



int hash_get (HashTable *hp, int k, void **vp){
    int location;
    node* curr;
    //lock
    location = simple_hash_function(k, hp->size); //bucket no of key k
    curr = hp->table[location];
    //create lock & locked
    int M = hp->size/hp->K;
    int func_lock = location / M;
    pthread_mutex_lock(&locks[func_lock]);

    
    while(curr != NULL){
        if(curr->key == k){
            //found key
            (*vp) = curr->value;
            //unlock
            pthread_mutex_unlock(&locks[func_lock]);
            return 0;
        }
        curr = curr->next;
    }
    //unlock
    pthread_mutex_unlock(&locks[func_lock]);
    return -1;
}

int sort_display(HashTable* hp){
    node* list = NULL;
    node* bucket = NULL;
    node* curr = NULL;
    int counter =0;
    for(int i = 0; i<hp->size; i++){
        bucket = hp->table[i];
        while(bucket != NULL){
            if(list == NULL){
                //if list is initially empty
                node* new_list_node = (struct key_value_pair*)malloc(sizeof(node));
                new_list_node->key = bucket->key;
                new_list_node->value = bucket->value;
                new_list_node->next = NULL;
                list = new_list_node;

            }
            else{
                    node* new_list_node = (struct key_value_pair*)malloc(sizeof(node));
                    new_list_node->key = bucket->key;
                    new_list_node->value = bucket->value;

                    if(curr->next == NULL){
                        //insert second to list
                        if(curr->key < bucket->key){
                            new_list_node->next = NULL; 
                            curr->next = new_list_node;
                        }
                        else{
                            new_list_node->next = curr;
                            list = new_list_node;
                        }
                    }
                    
                    else{
                        if(bucket->key < curr->key){
                            new_list_node->next = curr;
                            list = new_list_node;
                        }
                        else
                        {
                                while(curr->next != NULL){
                                    if(curr->next->key > bucket->key){
                                        new_list_node->next = curr->next;
                                        curr->next = new_list_node;
                                        break;
                                    }
                                    else{
                                        curr = curr->next;
                                    }
                                }

                                if(curr->next == NULL){
                                    curr->next = new_list_node;
                                    new_list_node->next = NULL;
                                }
                            
                        }
                    }
            }
            bucket = bucket->next;
            curr = list;

        }

    }

    //display 
    node* display_iter = list;
    while(display_iter != NULL){
        counter++;
        printf("%d: %d\n", display_iter->key, *(int*)(display_iter->value));
        display_iter = display_iter->next;
    }
    
    return counter;
}

/*
TEST FUNCTION
*/
int hash_display(HashTable *hp){

    if(hp == NULL){
        printf("Error: No such table\n");
        return -1;
    }
    else{
        pthread_mutex_t hdisplay_lock = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&hdisplay_lock);
        for (int i = 0; i < hp->size; ++i)
        {
            if (hp->table[i] != NULL)
            {
                node* curr = hp->table[i];
                printf("Bucket no: %d, elements: \n",i );

                while(curr != NULL){
                    printf("-> %d, value: %d\n", curr->key, *(int*)curr->value);
                    curr = curr->next;
                }
            }

        }
        pthread_mutex_unlock(&hdisplay_lock);
        return 0;
    }
}