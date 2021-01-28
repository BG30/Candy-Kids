#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "bbuff.h"
#include "stats.h"

//variable that will stop producer threads
_Bool stop_thread = false; 

//data structure for candy
typedef struct {
	int factory_number;
	double time_stamp_in_ms;
} candy_t;

//tracks time of creation
double current_time_in_ms(void){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

//shows time ticking away in program
void wait_for_req_time(int time){
	int i = 0;
	while( i < time){
		printf("Time %d s\n", i);
		sleep(1);
		i++;
	}
}

//creates a factory thread
void* create_producer(void* arg){
	//variable that holds time factory will wait
	int wait_t;
	//converting argument from pointer to int
	int num = *((int *)arg);
	//loop will execute until stop_thread is true in main()
	while(!stop_thread){
		wait_t = (rand() % (3-0+1)+0);
		printf("\tFactory %d ships candy & waits %ds\n", num, wait_t);
		
		candy_t* candy = malloc(sizeof (candy_t));
		candy->factory_number = num;
		candy->time_stamp_in_ms = current_time_in_ms();
		
		bbuff_blocking_insert(candy);
		stats_record_produced(num);
		
		sleep(wait_t);
	}
	//thread is done
	printf("Candy-factory %d done\n", num);
	pthread_exit(NULL);
}

//stop a factory thread
void stop_factories(int num_factories, pthread_t* factory_threads){
	for (int i = 0; i < num_factories; i++){
		pthread_join(factory_threads[i], NULL);
	}
}


//creates a kid thread
void *create_kid(){
	//variable will hold time kid will sleep
	int wait_t;
	//stores factory number of candy
	int factory_num;

	while(true){
		wait_t = (rand() % (1-0+1)+0);
		//extract candy from buffer
		candy_t* candy = bbuff_blocking_extract();
		factory_num = candy->factory_number;
		//record that candy is eaten		
		stats_record_consumed(factory_num, (current_time_in_ms() - candy->time_stamp_in_ms));
		//free the candy memory
		free(candy);
		//sleep the kid
		sleep(wait_t);
	}
}

//stop a kid thread
void stop_kids(int num_kids, pthread_t* kid_threads){
	for (int i = 0; i < num_kids; i++){
		pthread_cancel(kid_threads[i]);
		pthread_join(kid_threads[i], NULL);
	}
}

int main(int argc, char *argv[]){
	//checking if exactly 3 arguments are passed in
	if (argc != 3){                                //argc < 3 || argc > 4
		printf("Need exactly 3 arguments\n");
		exit(1);
	}

	//checking if all 3 arguments greater than 0
	for (int i = 1; i < 4; ++i){
		if(atoi(argv[i]) <= 0){
			printf("All arguments must be greater than 0\n");
			exit(1);
		}
	}

	//assigning input values
	int factories = atoi(argv[1]);
	int kids = atoi(argv[2]);
	int seconds = atoi(argv[3]);

	//initialize bounded buffer
	bbuff_init();
	
	//initialize statistics
	stats_init(factories);

	//launch factory
	
	//list of all factories
	pthread_t factory_threads[factories];
	//id of all factories
	int factory_id[factories];

	for (int i = 0; i < factories; i++){
		factory_id[i] = i;
		pthread_create(&factory_threads[i], NULL, create_producer, (void *)&factory_id[i]);
	}

	//launch kids
	
	//list of all kids
	pthread_t kid_threads[kids];
	
	for (int i = 0; i < kids; i++){
		pthread_create(&kid_threads[i], NULL, create_kid, NULL);
	}

	//waiting for requested time
	wait_for_req_time(seconds);

	//stop factroy threads
	stop_thread = true;
	stop_factories(factories, factory_threads);

	//wait until no candies 
	while(bbuff_is_empty() == false){
		printf("Waiting for all candy to be consumed\n\n");
		sleep(1);
	}

	//Stopping kids
	stop_kids(kids, kid_threads);

	//print statistics
	stats_display();
	
	//cleanup statistics module
	stats_cleanup();
}