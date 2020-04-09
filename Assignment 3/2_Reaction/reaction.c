#include "cs_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {

	// FILL ME IN
	int h_count; 
	int h_remaining_returns;

	struct condition h_not_needed, reaction_finished, h_returned;
	struct lock h_count_lock, reaction_going_on, remaining_returns_lock;


};

void reaction_init(struct reaction *new_reaction)
{
	// FILL ME IN	
	

	cond_init(&new_reaction->h_not_needed);
	cond_init(&new_reaction->reaction_finished);
	cond_init(&new_reaction->h_returned);


	lock_init(&new_reaction->h_count_lock);
	lock_init(&new_reaction->reaction_going_on);
	lock_init(&new_reaction->remaining_returns_lock);
	
	new_reaction->h_remaining_returns=0;
	new_reaction->h_count=0;


}

void reaction_h(struct reaction *reaction)
{
	// FILL ME IN
	lock_acquire(&reaction->h_count_lock);

	reaction->h_count++;

	if (reaction->h_count>=2){
		cond_signal(&reaction->h_not_needed, &reaction->h_count_lock);		//signalling to have a reaction with waiting O atoms to form water molecule
	}

	cond_wait(&reaction->reaction_finished, &reaction->h_count_lock);

	reaction->h_count--;												//as reaction is now finished so decreasing h count
	
	lock_acquire(&reaction->remaining_returns_lock);
	
	reaction->h_remaining_returns--;
	if (reaction->h_remaining_returns == 0){
		cond_signal(&reaction->h_returned, &reaction->remaining_returns_lock);
	}

	lock_release(&reaction->remaining_returns_lock);
	lock_release(&reaction->h_count_lock);
	
	return;


}

void reaction_o(struct reaction *reaction)
{
	// FILL ME IN
	lock_acquire(&reaction->reaction_going_on);
	lock_acquire(&reaction->remaining_returns_lock);

	while(reaction->h_remaining_returns>0){
		cond_wait(&reaction->h_returned, &reaction->remaining_returns_lock);
	}

	lock_acquire(&reaction->h_count_lock);

	while (reaction->h_count<2){
		cond_wait(&reaction->h_not_needed, &reaction->h_count_lock);
	}

	make_water();
	
	reaction->h_remaining_returns=2;

	lock_release(&reaction->remaining_returns_lock);
	
	cond_signal(&reaction->reaction_finished, &reaction->reaction_going_on);
	cond_signal(&reaction->reaction_finished, &reaction->reaction_going_on);
	
	lock_release(&reaction->h_count_lock);
	lock_release(&reaction->reaction_going_on);
	 
	return;


}