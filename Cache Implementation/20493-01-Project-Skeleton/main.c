/*
 * main.c
 *
 * 20493-01 Computer Architecture
 * Term Project on Implentation of Cache Mechanism
 *
 * Skeleton Code Prepared by Prof. HyungJune Lee
 * Nov 15, 2021
 *
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cache_impl.h"

#define MAX_LINE_SIZE 10

int num_cache_hits = 0;
int num_cache_misses = 0;

int num_bytes = 0;
int num_access_cycles = 0;

int global_timestamp = 0;

int retrieve_data(void *addr, char data_type) {
    int value_returned = -1; /* accessed data */

    /* Invoke check_cache_data_hit() */
    value_returned = check_cache_data_hit(addr, data_type);

    if (value_returned == -1) { //miss event
        num_cache_misses++;
        //access memory when miss event
        value_returned = access_memory(addr, data_type);
    }
    else //hit event
        num_cache_hits++;

    return value_returned;    
}

int main(void) {
    FILE *ifp = NULL, *ofp = NULL;
    unsigned long int access_addr; /* byte address (located at 1st column) in "access_input.txt" */
    char access_type; /* 'b'(byte), 'h'(halfword), or 'w'(word) (located at 2nd column) in "access_input.txt" */
    int accessed_data; /* This is the data that you want to retrieve first from cache, and then from memory */ 
    
    init_memory_content();
    init_cache_content();
    
    ifp = fopen("access_input.txt", "r");
    if (ifp == NULL) {
        printf("Can't open input file\n");
        return -1;
    }
    ofp = fopen("access_output.txt", "w");
    if (ofp == NULL) {
        printf("Can't open output file\n");
        fclose(ifp);
        return -1;
    }
    
    fputs("[Accessed Data]\n", ofp);

	/* read each line and get the data in given (address, type) by invoking retrieve_data() */

	while (1)
    {
        char *tmp;// temporary storage for line read from file.
        char line[MAX_LINE_SIZE]; // buffer space to copy the line.
		int i = 1;

		//get a line from the file
        tmp = fgets(line, MAX_LINE_SIZE, ifp);

        //when there is no line to read, break.
        if (tmp == NULL)
            break;

        //get the string before first " " and change it to int
		access_addr = atoi(strtok(tmp," "));

        //initialize access_type and find for data type using while loop
        access_type = tmp[0];
        while (!(access_type == 'w' || access_type == 'h' || access_type == 'b'))
            access_type = tmp[i++];

        //fetch data from cache or memory
		accessed_data = retrieve_data(access_addr, access_type);

        //write fetched information in file
		fprintf(ofp,"%d	%c	%#x\n", access_addr, access_type, accessed_data);
        global_timestamp++;
		
	}

	/* print hit ratio and bandwidth for each cache mecahanism as ragards to cache association size */
	fputs("-----------------------------------------\n", ofp);

	switch (DEFAULT_CACHE_ASSOC)    // select the output statement according to the assocativity.
	{
	case 1:
		fputs("[Direct mapped cache performance]\n", ofp);
		break;
	case 2:
		fputs("[2-way set associative cache performance]\n", ofp);
		break;
	case 4:
		fputs("[Fully associative cache performance]\n", ofp);
		break;
	default:
		fputs("error\n", ofp);
	}

    //calculate hit ratio and bandwidth
    float hit_ratio = (float)num_cache_hits / (float)(num_cache_hits + num_cache_misses);
    float bandwidth = (float)num_bytes / (float)num_access_cycles;

    //file write
	fprintf(ofp, "Hit ratio = %.2f (%d/%d)\n", hit_ratio, num_cache_hits, num_cache_hits + num_cache_misses);
	fprintf(ofp, "Bandwidth = %.2f (%d/%d)", bandwidth, num_bytes, num_access_cycles);

	/* close files */
    fclose(ifp);
    fclose(ofp);
    
    print_cache_entries();
    return 0;
}
