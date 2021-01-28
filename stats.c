#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "stats.h"

//mutex for entering and leaving the critical section
sem_t mutex;

//used to store the amount of factories
int length_data = 0;

//structure that will hold statistics
typedef struct {
	int factory_num;
	int candy_made;
	int candy_eaten;
	double min_delay;
	double avg_delay;
	double max_delay;
	double total_delay;
} stats;

//made a global pointer that will store the statistics array
stats *stat_data = NULL;

//initializes statistics module
void stats_init(int num_producers){
	//initialize stats struct
	sem_init(&mutex, 0, 1);
	length_data = num_producers;

	stat_data =(stats*) malloc(num_producers * sizeof(stats));
	for (int i = 0; i < num_producers; i++){
		stat_data[i].factory_num = i;
		stat_data[i].candy_made = 0;
		stat_data[i].candy_eaten = 0;
		stat_data[i].min_delay = 0.0;
		stat_data[i].max_delay = 0.0;
		stat_data[i].total_delay = 0.0;
	}
}

//cleans statistics data
void stats_cleanup(void){
	free(stat_data);
}

//records statistics for a produced candy
void stats_record_produced(int factory_number){
	sem_wait(&mutex);
	stat_data[factory_number].candy_made++;
	sem_post(&mutex);
}

//records statistics for consumed candy
void stats_record_consumed(int factory_number, double delay_in_ms){
	sem_wait(&mutex);
	
	stat_data[factory_number].candy_eaten++;
	
	//check if this is the first time editing stats
	if (stat_data[factory_number].min_delay == 0){
		stat_data[factory_number].min_delay = delay_in_ms;
		stat_data[factory_number].max_delay = delay_in_ms;
	}
	//not first time editing stats
	else{
		//editing stats for min_delay
		if (stat_data[factory_number].min_delay > delay_in_ms){
			stat_data[factory_number].min_delay = delay_in_ms;
		}
		//editing stats for max_delay
		if (stat_data[factory_number].max_delay < delay_in_ms){
			stat_data[factory_number].max_delay = delay_in_ms;
		}
	}

	stat_data[factory_number].total_delay = stat_data[factory_number].total_delay + delay_in_ms;
	
	sem_post(&mutex);
}

//displays statistics
void stats_display(void){
	//counting candies eaten and made
	int total_candy_made = 0;
	int total_candy_eaten = 0;

	for (int i = 0; i < length_data; i++){
		total_candy_made += stat_data[i].candy_made;
		total_candy_eaten += stat_data[i].candy_eaten;
	}
	
	//check if the number of candies made and eaten differ
	if (total_candy_eaten != total_candy_made){
		printf("ERROR: Mismatch between number made and eaten.\n");
		return;
	}
	
	printf("\nStatistics:\n");
	printf("%8s%8s%8s  %10s  %10s  %10s\n", 
		"Factory#", 
    	"#Made", 
	   	"#Eaten", 
	    "Min Delay[ms]", 
	    "Avg Delay[ms]", 
		"Max Delay[ms]"
	);

	for (int i = 0; i < length_data; i++){
		printf("%8d%8d%8d     %10.5f     %10.5f     %10.5f\n", 
			i, 
			stat_data[i].candy_made, 
			stat_data[i].candy_eaten, 
			stat_data[i].min_delay, 
			((stat_data[i].total_delay) / stat_data[i].candy_eaten), 
			stat_data[i].max_delay
		);
	}
	
}