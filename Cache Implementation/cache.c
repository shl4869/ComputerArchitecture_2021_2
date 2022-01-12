/*
 * cache.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implentation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 15, 2021
 *
 */


#include <stdio.h>
#include <string.h>
#include "cache_impl.h"

extern int num_cache_hits;
extern int num_cache_misses;

extern int num_bytes;
extern int num_access_cycles;

extern int global_timestamp;

cache_entry_t cache_array[CACHE_SET_SIZE][DEFAULT_CACHE_ASSOC];
int memory_array[DEFAULT_MEMORY_SIZE_WORD];


/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_memory_content() {
    unsigned char sample_upward[16] = {0x001, 0x012, 0x023, 0x034, 0x045, 0x056, 0x067, 0x078, 0x089, 0x09a, 0x0ab, 0x0bc, 0x0cd, 0x0de, 0x0ef};
    unsigned char sample_downward[16] = {0x0fe, 0x0ed, 0x0dc, 0x0cb, 0x0ba, 0x0a9, 0x098, 0x087, 0x076, 0x065, 0x054, 0x043, 0x032, 0x021, 0x010};
    int index, i=0, j=1, gap = 1;
    
    for (index=0; index < DEFAULT_MEMORY_SIZE_WORD; index++) {
        memory_array[index] = (sample_upward[i] << 24) | (sample_upward[j] << 16) | (sample_downward[i] << 8) | (sample_downward[j]);
        if (++i >= 16)
            i = 0;
        if (++j >= 16)
            j = 0;
        
        if (i == 0 && j == i+gap)
            j = i + (++gap);
            
        printf("mem[%d] = %#x\n", index, memory_array[index]);
    }
}   

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
void init_cache_content() {
    int i, j;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        for (j=0; j < DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            pEntry->valid = 0;
            pEntry->tag = -1;
            pEntry->timestamp = 0;
        }
    }
}

/* DO NOT CHANGE THE FOLLOWING FUNCTION */
/* This function is a utility function to print all the cache entries. It will be useful for your debugging */
void print_cache_entries() {
    int i, j, k;
    
    for (i=0; i<CACHE_SET_SIZE; i++) {
        printf("[Set %d] ", i);
        for (j=0; j <DEFAULT_CACHE_ASSOC; j++) {
            cache_entry_t *pEntry = &cache_array[i][j];
            printf("V: %d Tag: %#x Time: %d Data: ", pEntry->valid, pEntry->tag, pEntry->timestamp);
            for (k=0; k<DEFAULT_CACHE_BLOCK_SIZE_BYTE; k++) {
                printf("%#x(%d) ", pEntry->data[k], k);
            }
            printf("\t");
        }
        printf("\n");
    }
}

int check_cache_data_hit(void* addr, char type) {

    int access_addr = (int)(addr);

    //add cache access cycle time to the total access cycle
    num_access_cycles += CACHE_ACCESS_CYCLE;

    //calculate the block address for calculating the tag and set
    int block_address = access_addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE;

    //calculate the tag and set bit using block address and number of sets
    int tag = block_address / CACHE_SET_SIZE;
    int set = block_address % CACHE_SET_SIZE;

    //check if the data is in the cache
    for (int entry = 0; entry < DEFAULT_CACHE_ASSOC; entry++) {
        //when the valid bit is 1 and the tag of the cache entry matches the calculated tag,
        if (cache_array[set][entry].valid && cache_array[set][entry].tag == tag) {
            
            //update the time stamp of the cache entry
            cache_array[set][entry].timestamp = global_timestamp;
           
            //calculate the byte offset
            int byte_offset = access_addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE;

            //set the byte size of the data to fetch using switch
            int byte_size = 0;
            switch (type)
            {
            case 'w'://word
                byte_size = 4;
                break;

            case 'h'://half word
                byte_size = 2;
                break;

            case 'b'://byte
                byte_size = 1;
                break;

            default:
                byte_size = -1;
                fprintf(stderr,"error: Association type not defined");
                break;
            }

            //add byte size of accessed data to num_bytes
            num_bytes += byte_size;

            //copy the data from the cache entry
            int value_returned = 0;
            for (int j = byte_offset + byte_size - 1; j >= byte_offset; j--) {
                //shift 2 digits in hexadecimal and append the following value using or operation
                int digit = cache_array[set][entry].data[j] & 0xff; //prevent copying sign extension
                value_returned = (value_returned << 8) | digit;
                
            }
            //return data fetched from cache
            return value_returned;
        }
    }
    /* Return the data */
    //this return statement is executed when there is no matching data in the cache
    return -1;    
}

/* this function is to find the entry index in set for copying to cache */
int find_entry_index_in_set(int set)
{
    int entry_index;

    /* check if there exists any empty cache space by cheking 'valid' */
    for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++)
    {
        if (!cache_array[set][i].valid)
        {
            return i;
        }
    }
    /* if the set has only 1 entry, return index 0 */
    if (DEFAULT_CACHE_ASSOC == 1)   // case of 1-way set associative cache (direct mapped cache)
    {
        return 0;
    }
    /* otherwise, search over all entries to find the least recently used entry bu cheking 'timestamp' */
    else                          // case of 2/4-way set associative cache 
    {
        entry_index = 0;
        for (int i = 0; i < DEFAULT_CACHE_ASSOC; i++)     // find minimum timestamp(the one unused for the longest time)
        {
            if (cache_array[set][i].timestamp < cache_array[set][entry_index].timestamp)
            {
                entry_index = i;
            }
        }
    }
    /* return the cache index for copying from memory */
    return entry_index;
}

int access_memory(void *addr, char type) {
    
    int access_addr = (int)(addr);

    //add memory access cycle time to the total access cycle
    num_access_cycles += MEMORY_ACCESS_CYCLE;

    //calculate the block address for calculating the tag and set
    int block_address = access_addr / DEFAULT_CACHE_BLOCK_SIZE_BYTE;

    //calculate the tag and set bit using block address and number of sets
    int tag = block_address / CACHE_SET_SIZE;
    int set = block_address % CACHE_SET_SIZE;

    //get entry index to copy the data and paste to
    int entry_index = find_entry_index_in_set(set);

    //get the word address to use in the memory access
    int word_address = block_address * DEFAULT_CACHE_BLOCK_SIZE_BYTE / WORD_SIZE_BYTE;

    //exception: when the word address goes over the size of the memory
    if (DEFAULT_MEMORY_SIZE_WORD < word_address) {
        printf("error: Memory address out of range");
        return -1;
    }

    //set the cache entry for data copy from memory
    cache_array[set][entry_index].valid = 1;
    cache_array[set][entry_index].tag = tag;
    cache_array[set][entry_index].timestamp = global_timestamp;

    //copy a block(2words) from memory
    int mem_data = memory_array[word_address];
    for (int i = 0; i < DEFAULT_CACHE_BLOCK_SIZE_BYTE; i++) {
        if (i == DEFAULT_CACHE_BLOCK_SIZE_BYTE / 2) {
            mem_data = memory_array[word_address + 1];
        }

        //cut last two digits in hexadecimal form and save to the cache data location
        cache_array[set][entry_index].data[i] = mem_data % 256;
        mem_data >>= 8;
    }

    //calculate the byte offset
    int byte_offset = access_addr % DEFAULT_CACHE_BLOCK_SIZE_BYTE;

    //set the byte size of the data to fetch using switch
    int byte_size = 0;
    switch (type)
    {
    case 'w'://word
        byte_size = 4;
        break;

    case 'h'://half word
        byte_size = 2;
        break;

    case 'b'://byte
        byte_size = 1;
        break;

    default:
        byte_size = -1;
        fprintf(stderr,"error: Association type not defined");
        break;
    }

    num_bytes += byte_size;

    //copy the data from the cache entry
    int value_returned = 0;
    for (int j = byte_offset + byte_size - 1; j >= byte_offset; j--) {
        //shift 2 digits in hexadecimal and append the following value using or operation
        int digit = cache_array[set][entry_index].data[j] & 0xff; //prevent copying sign extension
        value_returned = (value_returned << 8) | digit;
    }

    //return fetched data from memory
    return value_returned;
}
