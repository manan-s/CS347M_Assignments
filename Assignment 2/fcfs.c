#include "sched.h"

void fcfs(struct processes *p, int np)
{
  // Sort processes in order of arrival time.
  sort(p, np);
  int wait_time = 0;
  int turn_time = 0;

  p[0].waiting_time = wait_time;
  p[0].turnaround_time = wait_time + p[0].burst_time;
  for(int i = 1; i < np; i++){
      turn_time += p[i-1].turnaround_time;
      wait_time = (wait_time + p[i-1].burst_time - p[i].arrival_time + p[i-1].arrival_time > 0)? wait_time + p[i-1].burst_time - p[i].arrival_time + p[i-1].arrival_time : 0;
      p[i].waiting_time = wait_time;
      p[i].turnaround_time = wait_time + p[i].burst_time;
  }
  turn_time+=p[np-1].turnaround_time;

  printf("\nProcessId, ArrivalTime, BurstTime, TurnaroundTime, WaitingTime \n");
    for(int i = 0; i < np; i++){
    printf("%d        , %d          , %d        , %d            , %d\n",
	   p[i].process_id,
	   p[i].arrival_time,
	   p[i].burst_time,
	   p[i].turnaround_time,
	   p[i].waiting_time);
  }
  int t_time = 0;
  
  for(int i=0; i<np;i++){
    t_time += p[i].waiting_time;
  }
  float avg_wait = t_time*1.0/np;
  float avg_turn = turn_time*1.0/np;
  printf("Average waiting time : %f \n",avg_wait);
  printf("Average turnaround time : %f \n",avg_turn);
}

