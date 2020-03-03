#include "sched.h"

void sort(struct processes *p ,int np)
{
  // Sorts processes by arrival time.
  struct processes t;
  for(int i = 0; i < np -1 ;i++){
    for(int j = 0 ;j < np -i-1; j++)
      {
	if(p[j+1].arrival_time < p[j].arrival_time)
	  {
	    t = p[j+1];
	    p[j+1] = p[j];
	    p[j] = t;
	  }
      }
  }
}

void sort_rem(struct processes *p ,int np){
  // Sorts processes by remaining time with highest remaining time first. 
  struct processes t;
  for(int i = 0; i < np -1 ;i++) 
    {
      for(int j = 0 ;j < np -i-1; j++)
	{
	  if(p[j+1].remaining_time < p[j].remaining_time)
	    {
	      t = p[j+1];
	      p[j+1] = p[j];
	      p[j] = t;
	    }
	}
    }
}

int get_proc_id(struct processes *p ,int curr_time, int np){

	int sum = 0;
	int temp_id=0;

	int *id = malloc(sizeof(int)*np);
	int *ticket = malloc(sizeof(int)*np);

	for(int i=0;i<np;i++){
		id[i]= -1;
		ticket[i]= -1;
	}

	int i=0;
	while(i<np && p[i].arrival_time<=curr_time){
		
		if(p[i].burst_time==0){
			i++;
			continue;
		}

		sum+=p[i].ticket;
		id[temp_id]=i;
		ticket[temp_id]=p[i].ticket;
		temp_id++;
		i++;
	}

	if(sum==0)
		return INT_MAX;

	int random = rand()%sum;
	int range = 0, j=0;

	while(range<sum){

		if (random >= range&&random < range+ticket[j]){
			int return_value = id[j];
			free(id);
			free(ticket);
			return return_value;
		}
		else{
			range+=ticket[j];
			j++;
		}
	}
}

int get_proc_id_sjf(struct processes *p ,int curr_time, int np){
	
	int min_job = INT_MAX, min_id=INT_MAX;
	int i = 0;

	while (i<np && p[i].arrival_time<=curr_time){
		
		if(p[i].burst_time==0){
			i++;
			continue;
		}

		if(p[i].burst_time<min_job){
			min_job = p[i].burst_time;
			min_id = i;
		}
		i++;
	}
	return min_id;
}
