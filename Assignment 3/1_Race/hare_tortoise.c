#include "cs_thread.h"

// IDEA:
// A global clock is maintained
// Before every tick:
//
//	 First, tortoise moves
//	 Then the hare
//	 Then god repositions, if he has to
//	 The reporter reports data after the tick happens

struct Repositioning {
	char player; 		
	int time; 	
	int distance;		
};

struct race {
	
	int printing_delay;
	int tortoise_speed;					
	int hare_speed;						
	int hare_sleep_time; 				
	int hare_turtle_distance_for_sleep; 
	int finish_distance;			
	struct Repositioning* reposition;	
	int repositioning_count;
	char winner;
	
	int hare_position;
	int turtle_position;
	int hare_time;
	int turtle_time;
	int times_printed;

	int global_time;
	int whos_turn;   // 0 for tortoise, 1 for hare, 2 for god, 3 for reporter
	int is_finished;
	int turtle_finished;
	int hare_finished;

	int is_hare_sleeping;
	int remaining_sleep_time;
	int bye_reporter;
	int bye_god;
};

struct lock hare_lock, turtle_lock, god_lock, reporter_lock;
struct condition turtle_turn, hare_turn, god_turn, reporter_turn;

void* Turtle(struct race *race);
void* Hare(struct race *race);
void* God(struct race *race);
void* Report(struct race *race);

char init(struct race *race)
{

	race->turtle_position = 0;
	race->hare_position = 0;
	race->turtle_time = 0;
	race->hare_time = 0;
	race->times_printed = 0;
	race->global_time = 0;
	race-> whos_turn = 0;
	race->is_finished = 0;
	race->turtle_finished = 0;
	race->hare_finished = 0;
	race->is_hare_sleeping = 0;
	race->remaining_sleep_time = 0;
	race->bye_god = 0;
	race->bye_reporter = 0;

	lock_init(&hare_lock);
	lock_init(&turtle_lock);
	lock_init(&god_lock);
	lock_init(&reporter_lock);

	cond_init(&turtle_turn);
	cond_init(&hare_turn);
	cond_init(&god_turn);
	cond_init(&reporter_turn);

	pthread_t hare, turtle, god, reporter;
	
	printf("%s %d\n","Time now :", race->global_time);
	printf("%s %d\n","Tortoise :", race->turtle_position);
	printf("%s %d\n\n","Hare :", race->hare_position);

	pthread_create(&hare, NULL, Hare, race);
	pthread_create(&turtle, NULL, Turtle, race);
	pthread_create(&god, NULL, God, race);
	pthread_create(&reporter, NULL, Report, race);

	pthread_join(hare, NULL);
	pthread_join(turtle, NULL);
	pthread_join(reporter, NULL);	
	pthread_join(god, NULL);

	return race->winner; 
}

void* Turtle(struct race *race)
{
	while(race->is_finished!=1 && race->turtle_position < race->finish_distance){

		lock_acquire(&turtle_lock);

		while(race->whos_turn != 0){
			cond_wait(&turtle_turn, &turtle_lock);
		}

		lock_acquire(&god_lock);
		lock_acquire(&hare_lock);
		lock_acquire(&reporter_lock);

		race->turtle_position += race->tortoise_speed;

		if(race->turtle_position >= race->finish_distance){
			race->turtle_finished = 1;
			race->bye_god = 1;
			race->bye_reporter = 1;
		}

		race->turtle_time++;
		race->whos_turn = 1;
		cond_signal(&hare_turn, &hare_lock);

		lock_release(&god_lock);
		lock_release(&hare_lock);
		lock_release(&reporter_lock);
		lock_release(&turtle_lock);
	}
	return NULL;
}

void* Hare(struct race *race)
{

	while(race->is_finished!=1 && race->hare_position < race->finish_distance){
		
		lock_acquire(&hare_lock);

		while(race->whos_turn != 1){
				cond_wait(&hare_turn, &hare_lock);
			}
		
		lock_acquire(&turtle_lock);
		lock_acquire(&god_lock);
		lock_acquire(&reporter_lock);
		
		if(race->is_hare_sleeping != 1){
			
			if(race->is_finished!=1 && (race->hare_position - race->turtle_position) >= race->hare_turtle_distance_for_sleep){
				race->is_hare_sleeping = 1;
				race->remaining_sleep_time = race->hare_sleep_time;
				if(race->turtle_finished == 1){
					race->winner = 'T';
					race->is_finished = 1;
					race->bye_god = 1;
					race->bye_reporter = 1;
				}
			}

			else{
				race->hare_position += race->hare_speed;
				race->hare_time ++;

				if(race->hare_position >= race->finish_distance){
					race->hare_finished = 1;
					race->is_finished = 1;
					race->bye_god = 1;
					race->bye_reporter = 1;
				}

				if(race->turtle_finished == 1 && race->hare_finished == 1){
					if(race->hare_position == race->turtle_position)
						race->winner = 'B';
					else if(race->hare_position > race->turtle_position)
						race->winner = 'H';
					else
						race->winner = 'T';
				}

				else if(race->turtle_finished == 1){
					race->winner = 'T';
					race->is_finished = 1;
				}

				else if(race->hare_finished == 1){
					race->winner = 'H';
					race->is_finished = 1;
				}
			}
		}

		else{
			race->hare_time++;
			race->remaining_sleep_time--;
			if(race->remaining_sleep_time == 0){
				race->is_hare_sleeping = 0;
			}

			if(race->turtle_finished == 1){
				race->winner = 'T';
				race->is_finished = 1;
				race->bye_god = 1;
				race->bye_reporter = 1;
			}
		}

		race->whos_turn = 2;
		cond_signal(&god_turn, &god_lock);

		lock_release(&turtle_lock);
		lock_release(&god_lock);
		lock_release(&reporter_lock);
		lock_release(&hare_lock);
	}
	return NULL;
}

void* God(struct race *race)
{
	while(race->bye_god!=1 &&race->is_finished!=1){

		lock_acquire(&god_lock);

		while(race->whos_turn != 2){
			cond_wait(&god_turn, &god_lock);
		}

		lock_acquire(&turtle_lock);
		lock_acquire(&hare_lock);
		lock_acquire(&reporter_lock);
		
		for(int i=0; i < race->repositioning_count; i++)
		{
			if(race->global_time + 1 == (race->reposition)[i].time)
			{
				if((race->reposition)[i].player =='T')
				{
					race->turtle_position += (race->reposition)[i].distance;
					if(race -> turtle_position < 0) race->turtle_position = 0;
				}
				else
				{
					race->hare_position += (race->reposition)[i].distance;
					if(race -> hare_position < 0) race->hare_position = 0;
				}
			}
		}

		race->whos_turn = 3;

		cond_signal(&reporter_turn, &reporter_lock);
		lock_release(&turtle_lock);
		lock_release(&hare_lock);
		lock_release(&reporter_lock);
		lock_release(&god_lock);
	}
	return NULL;
}

void* Report(struct race *race)
{	
	while(race->bye_reporter!=1 && race->is_finished!=1){
		
		lock_acquire(&reporter_lock);

		while(race->whos_turn != 3){
			cond_wait(&reporter_turn, &reporter_lock);
		}

		lock_acquire(&turtle_lock);
		lock_acquire(&hare_lock);
		lock_acquire(&god_lock);

		printf("%s %d\n","Time now :", race->global_time + 1);
		printf("%s %d\n","Tortoise :", race->turtle_position);
		printf("%s %d\n\n","Hare :", race->hare_position);

		race->whos_turn = 0;
		race->global_time++;
		cond_signal(&turtle_turn, &turtle_lock);

		lock_release(&turtle_lock);
		lock_release(&hare_lock);
		lock_release(&god_lock);
		lock_release(&reporter_lock);
	}
	return NULL;
}
