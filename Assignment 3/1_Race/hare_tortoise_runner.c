#include <time.h>
#include <errno.h>  
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int sleep_ms(int milliseconds);

#include "hare_tortoise.c"

int sleep_ms(int milliseconds)
{
    struct timespec ts;
    int res;

    if (milliseconds < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

char w;

int parseInput(const char* filename, struct race * race) {
    FILE* f = fopen(filename,"r");
    if (f == NULL) {
        printf("Couldn't open file at %s\n",filename);
        return -1;
    }
    if (fscanf(f,"%d\n",&(race->printing_delay)) != 1) { printf("wrong input file format\n"); return -1;};
    if (fscanf(f,"%d\n",&(race->tortoise_speed)) != 1) { printf("wrong input file format\n"); return -1;};
    if (fscanf(f,"%d\n",&(race->hare_speed)) != 1) { printf("wrong input file format\n"); return -1;};
    if (fscanf(f,"%d\n",&(race->hare_sleep_time)) != 1) { printf("wrong input file format\n"); return -1;};
    if (fscanf(f,"%d\n",&(race->hare_turtle_distance_for_sleep)) != 1) { printf("wrong input file format\n"); return -1;};
    if (fscanf(f,"%d\n",&(race->finish_distance)) != 1) { printf("wrong input file format\n"); return -1;};
    if (fscanf(f,"%d\n",&(race->repositioning_count)) != 1) { printf("wrong input file format\n"); return -1;};
    race->reposition = (struct Repositioning*) malloc(sizeof(struct Repositioning) * race->repositioning_count);
    size_t i;
    for (i = 0; i < race->repositioning_count; i++)
    {
        // printf("%d\n",fscanf(f,"%c %d %d\n",
        //                 &((race->reposition)[i].player),
        //                 &((race->reposition)[i].time),
        //                 &((race->reposition)[i].distance)));
        if (fscanf(f,"%c %d %d\n",
                        &((race->reposition)[i].player),
                        &((race->reposition)[i].time),
                        &((race->reposition)[i].distance)) != 3) 
            { printf("wrong input file format\n"); return -1;};
    }

    printf("+------------------------------+\n");
    printf("| Printing delay  : %7d    |\n",race->printing_delay);
    printf("| Turtle speed    : %7d    |\n",race->tortoise_speed);
    printf("| Hare speed      : %7d    |\n",race->hare_speed);
    printf("| Hare Sleep time : %7d    |\n",race->hare_sleep_time);
    printf("| Hare-Turtle Dist: %7d    |\n",race->hare_turtle_distance_for_sleep);
    printf("| Total distance  : %7d    |\n",race->finish_distance);
    printf("+------------------------------+\n");
    printf("| Repositioning Data           |\n");
    printf("+--------+--------+------------+\n| player |  time  |  distance  |\n+--------+--------+------------+\n");
    for (i = 0; i < race->repositioning_count; i++) {
        printf("| %6c | %6d | %9d  |\n",
                        ((race->reposition)[i].player),
                        ((race->reposition)[i].time),
                        ((race->reposition)[i].distance));
    }
    if (fscanf(f,"%c\n",&(w)) != 1) { printf("wrong input file format\n"); return -1;};
    printf("+--------+--------+------------+\n");
    printf("+--------- Race Begins --------+\n");
    fclose(f);
    return 0;
}

int main(int argc, char const *argv[])
{
    struct race *race = malloc(sizeof(struct race));
    char* testFile  = "test-file=";
    const char* inputFileName = NULL;

    int i;

    for (i=0;i<argc;i++){

        if(!strncmp(testFile,argv[i],strlen(testFile)) && strlen(argv[i]) > strlen(testFile)){
            inputFileName = (argv[i] + strlen(testFile));
        }
    }

    if (inputFileName == NULL) {
        printf("Invalid file format.\n");
        return 0;
    }

    printf("Entered filename : %s\n",inputFileName);

    if (parseInput(inputFileName, race)) {return 0;}
    
    char winner = init(race);
    
    printf("+---------- Race Ends ---------+\n");
    
    if (winner == w) printf("Correct output.\n");
    else            printf("Wrong output.\nExpected output was %c.\nYou output was %c.\n",w,winner);
    
    return 0;
}
