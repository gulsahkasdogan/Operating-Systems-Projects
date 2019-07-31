Elif Gülşah Kaşdoğan – 21601183
Sıla İnci – 21602415

This project can be compiled by "make" in its directory. 

The integer-count.c application can be invoked by eg. "./integer-count 2 1.txt 2.txt out.txt"
In the out.txt file, there are the key values and the number of occurrences of that key in the .txt files given when the app is invoked. 

In our own hash library, we have the assigned functions as well as 3 other helper functions. 
1.keyExist (checks if the key exists in the hash)
2.hash_display (displays the buckets and the respective values in the hash)
3.sort_display (displays the unique keys and the number of times it occurrs in the file)

In test.c we test each hash function with various values of W,K,T and N. We calculate the time each operation takes and output it to terminal. Each thread performs all hash functions on the shared variable "hash_table". 

