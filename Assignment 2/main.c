#include "sched.h"


int main(int argc, char** argv){
  int no_of_processes;
  printf("Enter number of processes: ");
  scanf("%d", &no_of_processes);
  int choice;
  struct processes *p = malloc(sizeof(struct processes)*no_of_processes);
  
  for(int i = 0; i < no_of_processes; i++){
      printf("\nEnter Process ID  : ");
      scanf("%d",&p[i].process_id);
      printf("Enter Burst time  : ");
      scanf("%d",&p[i].burst_time);
      printf("Enter Arrival time: ");
      scanf("%d",&p[i].arrival_time);
  }

      while(choice!=5){
          printf("\n\t\t Main Menu ");
          printf("\n\t 1. First Come First Serve(FCFS)");
          printf("\n\t 2. STCF (Preemptive)");
          printf("\n\t 3. SJF (Non-Preemptive)");
          printf("\n\t 4. Lottery Scheduling (Preemptive)");
          printf("\n\t 5. Exit\n");
          printf("\nEnter choice: ");
          scanf("%d",&choice);
          int tickets, quantum, sum = 0;

          switch(choice){

            case 1: fcfs(p,no_of_processes);
              break;
            
            case 2: stcf(p,no_of_processes);
              break;
            
            case 3: sjf(p,no_of_processes);
              break;
            
            case 4: 
              
              printf("\nEnter value of quantum: ");
              scanf("%d",&quantum);
              printf("\n");

              for(int i = 0; i < no_of_processes; i++){
                printf("Enter tickets for Process ID ");
                printf("%d: ", p[i].process_id);
                scanf("%d", &p[i].ticket);
                sum+=p[i].ticket;
              }

              printf("\n");
              lottery(p,no_of_processes,quantum);
              break;
              
            case 5: printf("\nGoodbye!\n");
              free(p);
              exit(0);
              
            default:
              printf("\nDont get that choice\n");
              free(p);
              exit(0);
          }
      }
  printf("Hey\n");
  free(p);
  exit(0);
  
}
