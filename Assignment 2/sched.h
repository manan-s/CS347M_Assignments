#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

struct processes
{
	int arrival_time;
	int burst_time;
	int process_id;
	int turnaround_time;
	int waiting_time;
	int remaining_time;
	int leave_time;
	int ticket;
};

void sort(struct processes *p ,int np);
void sort_rem(struct processes *p ,int np);
int get_proc_id(struct processes *p ,int curr_time, int np);
int get_proc_id_sjf(struct processes *p ,int curr_time, int np);

void fcfs(struct processes *p, int np);
void sjf(struct processes *p, int np);
void stcf(struct processes *p, int np);
void lottery(struct processes *p, int np, int quantum);
