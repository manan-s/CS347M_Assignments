#include "sched.h"

void lottery(struct processes *p, int np,  int quantum)
{
  // Sort processes in order of arrival time.
    sort(p, np);
    int total_burst_time = 0;
    bool *hasrun = malloc(sizeof(bool)*np);
    int *init_burst_time = malloc(sizeof(int)*np);

    for(int i = 0; i < np; i++){
        total_burst_time+=p[i].burst_time;
        init_burst_time[i]=p[i].burst_time;
        hasrun[i]=0;
    }

    int start_time = p[0].arrival_time;
    int curr_time = p[0].arrival_time;

    while(total_burst_time!=0){

        int curr_id = get_proc_id(p,curr_time,np);  //get the ID to run

        if(curr_id==INT_MAX){
           int i=0;
           while(i<np&&p[i].burst_time==0){
             i++;
           }
           curr_id = i;
           curr_time = p[i].arrival_time;
        }
        
        if(p[curr_id].burst_time==0) continue;

        if(!hasrun[curr_id]){
            hasrun[curr_id]=1;
        }

        int exec_time = (p[curr_id].burst_time > quantum)? quantum: p[curr_id].burst_time;
        //printf("Process %d ran for %d  time\n", curr_id, exec_time);
        total_burst_time -= exec_time;
        curr_time+=exec_time;
        //printf("Curr Time: %d\n", curr_time);
        p[curr_id].burst_time -= exec_time;

        if(!p[curr_id].burst_time) {
            p[curr_id].turnaround_time = curr_time - p[curr_id].arrival_time;
        }     
    }

    //Restore burst time
    for(int i = 0; i < np; i++){
        p[i].burst_time = init_burst_time[i];
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
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
    int wait_time = 0, turn_time = 0;

    for(int i=0; i<np;i++){
    wait_time += p[i].waiting_time;
    turn_time += p[i].turnaround_time;
    }
    float avg_wait = wait_time*1.0 /np;
    float avg_turn = turn_time*1.0 /np;
    printf("\nAverage waiting time : %f \n",avg_wait);
    printf("Average turnaround time : %f \n",avg_turn);
    free(hasrun);
    free(init_burst_time);
}