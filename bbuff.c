#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdbool.h>
#include "bbuff.h"

//defining semaphores
sem_t fullBuff;
sem_t emptyBuff;

//defining lock
sem_t lock;

//conditional variable
int read_count;

//defining bounded buffer 
void* bounded_buffer [BUFFER_SIZE];

//intializing the bounded buffer
void bbuff_init(void){
	read_count = 0;
	sem_init(&fullBuff, 0, 0);
	sem_init(&emptyBuff, 0, BUFFER_SIZE);
	sem_init(&lock, 0, 1);
}

//Producer: adding candy to bounded buffer
void bbuff_blocking_insert(void* item){
	sem_wait(&emptyBuff);
	sem_wait(&lock);

	//inserting
	bounded_buffer[read_count] = item;
	read_count++;

	sem_post(&lock);
	sem_post(&fullBuff);
}

//Consumer: eat candy from bounded buffer
void* bbuff_blocking_extract(void){
	sem_wait(&fullBuff);
	sem_wait(&lock);

	//extracting
	read_count --;
	void *candy = bounded_buffer[read_count];

	sem_post(&lock);
	sem_post(&emptyBuff);
	
	//return candy
	return candy;
}

//check if bounded buffer is empty
_Bool bbuff_is_empty(void){
	if (read_count == 0)
		return true;
	else
		return false;
}