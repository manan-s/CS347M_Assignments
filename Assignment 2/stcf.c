#include "sched.h"

void stcf(struct processes *p, int np){
			int count = 0;
                  
                  int *temp = malloc(sizeof(int)*(np+1));
			for(int i = 0; i<np; i++){
				temp[i] = p[i].burst_time;
			}
			temp[np] = __INT_MAX__;
			int smallest;
      for(int time = 0; count != np; time++)
      {
            smallest = np;
            for(int i = 0; i < np; i++)
            {
                  if(p[i].arrival_time <= time && temp[i] < temp[smallest] && temp[i] > 0)
                  {
                        smallest = i;
                  }
            }
            temp[smallest]--;
            if(temp[smallest] == 0)
            {
                  count++;
                  p[smallest].leave_time = time + 1;
                  p[smallest].waiting_time = p[smallest].leave_time - p[smallest].arrival_time - p[smallest].burst_time;
                  p[smallest].turnaround_time = p[smallest].leave_time - p[smallest].arrival_time;
            }
			}
      sort(p, np);
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
    t_time += p[i].waiting_time;
		turn_time += p[i].turnaround_time;
  }
  float avg_wait = 1.0*t_time /np;
  float avg_turn = 1.0*turn_time /np;
  printf("\nAverage waiting time : %f \n",avg_wait);
  printf("Average turnaround time : %f \n",avg_turn);
  free(temp);
}

