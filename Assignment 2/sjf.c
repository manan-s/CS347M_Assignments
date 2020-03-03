#include "sched.h"

void sjf(struct processes *p, int np){
    
    sort(p, np);
    int total_burst_time = 0;
    int *init_burst_time = malloc(sizeof(int)*np);

    for(int i = 0; i < np; i++){
        total_burst_time+=p[i].burst_time;
        init_burst_time[i]=p[i].burst_time;
    }

    int start_time = p[0].arrival_time;
    int curr_time = p[0].arrival_time;

    while(total_burst_time!=0){

        int curr_id = get_proc_id_sjf(p,curr_time,np);  //get the ID to run
        
        if(curr_id==INT_MAX){
           int i=0;
           while(i<np&&p[i].burst_time==0){
             i++;
           }
           curr_id = i;
           curr_time = p[i].arrival_time;
        }
        
        total_burst_time -= p[curr_id].burst_time;
        curr_time += p[curr_id].burst_time;
        p[curr_id].turnaround_time = curr_time - p[curr_id].arrival_time;
        p[curr_id].waiting_time = p[curr_id].turnaround_time - p[curr_id].burst_time;
        p[curr_id].burst_time = 0; 
    }

    for(int i = 0; i < np; i++){
        p[i].burst_time = init_burst_time[i];
    }

    printf("\nProcessId, ArrivalTime, BurstTime, TurnaroundTime, WaitingTime \n");
    for(int i = 0; i < np; i++){
    printf("%d        , %d          , %d        , %d            , %d\n",
        p[i].process_id,
        p[i].arrival_time,
        p[i].burst_time,
        p[i].turnaround_time,
        p[i].waiting_time);
    }

  int t_time = 0, turn_time = 0;

  for(int i=0; i<np;i++){
    t_time += p[i].waiting_time;;
		turn_time += p[i].turnaround_time;
  }
  float avg_wait = 1.0*t_time /np;
  float avg_turn = 1.0*turn_time /np;
  printf("Average waiting time : %f \n",avg_wait);
  printf("Average turnaround time : %f \n",avg_turn);
  free(init_burst_time);
}
