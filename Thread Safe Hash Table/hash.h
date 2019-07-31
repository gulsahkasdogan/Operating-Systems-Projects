/*
CS342 Project 2
Authors: Elif Gülşah Kaşdoğan – 21601183
         Sıla İnci – 21602415
Date: 29.03.2019
*/
#ifndef PROJECT2_LIBRARY_H
#define PROJECT2_LIBRARY_H

struct key_value_pair{
    int key;
    void* value;
    struct key_value_pair* next; // this will point to next element in linked list
    //since we are using chaining for collision resolution.
};

struct hash_table{
    int size; //N
    int K;
    struct key_value_pair** table;
};

typedef struct hash_table HashTable;
typedef struct key_value_pair node;

HashTable *hash_init (int N, int K);
int hash_insert (HashTable *hp, int k, void * v);
int hash_delete (HashTable *hp, int k);
int hash_update (HashTable *hp, int k, void *v);
int hash_get (HashTable *hp, int k, void **vp);
int hash_destroy (HashTable *hp);
/*TEST FUNCTION*/
int keyExist(node *list, int k);
int hash_display(HashTable *hp);
int sort_display(HashTable* hp);

#endif