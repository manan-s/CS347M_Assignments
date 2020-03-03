#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h> 
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1000
#define SHELL_ERROR printf("ERROR: Illegal command or arguments\n")
#define FORK_ERROR  printf("ERROR: Failed to fork process\n")
#define FILE_ERROR  printf("Error opening the file. Please check the PID or use \'top\'.\n")
#define ARG_ERROR   printf("ERROR: Missing arguments\n")

char **tokenize(char *line);

int parallel_check(char **words){
    int i;
    for(i=0; words[i]!=NULL; i++){
         if(!strcmp(words[i], ";"))
            return i;
    }
    return i;
}

int semicolon_check(char **words){
    int i;
    for(i=0;words[i]!=NULL;i++){
        for(int j=0; words[i][j]!='\0'; j++){
            if(words[i][j]==';') return 1;
        }
    }
    return 0;
}

void checkcpupercentage(char *pid){
    char temp[20];
    strcpy(temp, "/proc/");
    strcat(temp, pid);
    strcat(temp, "/stat");
    
    //##########Getting utime_before, stime_before#########################
    FILE *fp = fopen(temp, "r");
    int totalRead = 0;
    char buffer[BUFFER_SIZE];
    if(fp == NULL)
        FILE_ERROR;
    
    while(fgets(buffer, BUFFER_SIZE, fp) != NULL) 
    {
        totalRead = strlen(buffer);
        buffer[totalRead - 1] = buffer[totalRead - 1] == '\n' ? '\0' : buffer[totalRead - 1];
    }

    //Uncomment for printing /proc/[pid]/stat before sleeping
    //printf("%s\n", buffer);

    char **data = tokenize(buffer);

    int utime_before = atoi(data[13]), stime_before = atoi(data[14]);

    //##########Getting total_time_before#########################
    FILE *fp1 = fopen("/proc/stat", "r");
    char buffer1[BUFFER_SIZE];
    int totalRead1 = 0;
    if(fp1 == NULL)
        FILE_ERROR;
    while(fgets(buffer1, BUFFER_SIZE, fp1) != NULL) 
    {
        totalRead1 = strlen(buffer1);
        buffer1[totalRead1 - 1] = buffer1[totalRead1 - 1] == '\n' ? '\0' : buffer1[totalRead1 - 1];
        break;
    } 
    
    char **data1 = tokenize(buffer1);

    int total_time_before = 0;
    for(int i=1; data1[i]!=NULL; i++)
        total_time_before += atoi(data1[i]);
    
    sleep(1);

    //##########Getting utime_after, stime_after#########################
    FILE *fp2 = fopen(temp, "r");
    int totalRead2 = 0;
    char buffer2[BUFFER_SIZE];
    if(fp2 == NULL)
        FILE_ERROR;
    
    while(fgets(buffer2, BUFFER_SIZE, fp2) != NULL) 
    {
        totalRead2 = strlen(buffer2);
        buffer2[totalRead2 - 1] = buffer2[totalRead2 - 1] == '\n' ? '\0' : buffer2[totalRead2 - 1];
    }

    //Uncomment for printing /proc/[pid]/stat after sleeping
    //printf("%s\n", buffer2);

    char **data2 = tokenize(buffer2);

    int utime_after = atoi(data2[13]), stime_after = atoi(data2[14]);

    //##########Getting total_time_after#########################
    FILE *fp3 = fopen("/proc/stat", "r");
    char buffer3[BUFFER_SIZE];
    int totalRead3 = 0;
    if(fp3 == NULL)
        FILE_ERROR;
    while(fgets(buffer3, BUFFER_SIZE, fp3) != NULL) 
    {
        totalRead3 = strlen(buffer3);
        buffer3[totalRead3 - 1] = buffer3[totalRead3 - 1] == '\n' ? '\0' : buffer3[totalRead3 - 1];
        break;
    } 

    char **data3 = tokenize(buffer3);

    int total_time_after = 0;
    for(int i=1; data3[i]!=NULL; i++)
        total_time_after += atoi(data3[i]);
    
    double user_time = 100.0*(utime_after - utime_before + 0.0)/(total_time_after - total_time_before);
    double sys_time = 100.0*(stime_after - stime_before + 0.0)/(total_time_after - total_time_before);
    printf("\nUser Mode CPU Percentage: %E", user_time);
    printf("\nSystem Mode CPU Percentage: %E\n", sys_time);

    exit(0);
}

void checkresidentmemory(char *pid){
    
    char *args[] = {"/bin/ps", "-p", pid, "-o", "rss=", NULL};
    execve( args[0], args, NULL );

    exit(errno);
}

void listFiles(){
    int fd;
    system("touch files.txt");
    if((fd = open("files.txt", O_WRONLY|O_TRUNC)) < 0){
        printf("Error opening the file.\n");
    }
    
    int std_temp = dup(STDOUT_FILENO);
    dup2(fd, STDOUT_FILENO);

    char *args[2];
    args[0] = "/bin/ls";     
    args[1] = NULL;
    execve( args[0], args, NULL);

    fflush(stdout);
    dup2(std_temp, STDOUT_FILENO);
    exit(0);
}

void sortFile(char *file){
    
    dup2(STDIN_FILENO, 200);
    close(STDIN_FILENO);
    int fd = open(file, O_RDONLY);

    char *args[3];
    args[0] = "/usr/bin/sort";  
    args[1] = file;  
    args[2] = NULL;
    execve( args[0], args, NULL);

    close(fd);
    dup(200);
    close(200);
    exit(0);
}

int main(){
    char input[1024];
    bool exit = 0;
    pid_t pid1 = getpid();
    
    while (1){
        
        printf("myshell> ");
        memset(input, 1024, sizeof(input));   
        fgets(input,1024,stdin);
        char **words = tokenize(input);
        int parallel = parallel_check(words);
        int first_fail = 0;

        int status_t = 0, sort_flag = 0;
        pid_t wpid;

        if(semicolon_check(words)==1&&words[parallel]==NULL){
            printf("INVALID FORMAT: Semicolon must be preceded and followed by a space\n");
            continue;
        }

        for(int i=0; i<parallel ;i++){
            if(!strcmp(words[i], "checkcpupercentage")){
                int status = 0;
                pid_t pid;
                
                if(words[i+1]==NULL || !strcmp(words[i+1], ";") ){
                    if(words[parallel]==NULL) ARG_ERROR;
                    else printf("ERROR: Missing arguments in first command\n");
                    break;
                }
                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    checkcpupercentage(words[i+1]);
                }
                wait(&status);
                i++;
            }

            else if(!strcmp(words[i], "checkresidentmemory")){
                int status = 0;
                pid_t pid;
                
                if(words[i+1]==NULL || !strcmp(words[i+1], ";")){
                    if(words[parallel]==NULL) ARG_ERROR;
                    else printf("ERROR: Missing arguments in first command\n");
                    break;
                }
                
                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    checkresidentmemory(words[i+1]);
                }

                wait(&status);
                i++;
            }

            else if(!strcmp(words[i], "listFiles")){
                    
                pid_t pid;
                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    listFiles();
                }
            }

            else if(!strcmp(words[i], "sortFile")){
                int status = 0;
                pid_t pid;
                
                if(words[i+1]==NULL || !strcmp(words[i+1], ";")){
                    if(words[parallel]==NULL) ARG_ERROR;
                    else printf("ERROR: Missing arguments in first command\n");
                    break;
                }
                
                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    sortFile(words[i+1]);
                }
                wait(&status);
                i++;
            }

            else if(!strcmp(words[i], "listfiles")||!strcmp(words[i], "list_files")||!strcmp(words[i], "listfile")||!strcmp(words[i], "listFile")||!strcmp(words[i], "list")||!strcmp(words[i], "ls")){
                printf("ERROR: No such command exists. Did you mean \"listFiles\"?\n");        
            }

            else if(!strcmp(words[i], "sort")||!strcmp(words[i], "sortfiles")||!strcmp(words[i], "sortfile")){
                printf("ERROR: No such command exists. Did you mean \"sortFile\"?\n");
                i++;        
            }

            else if(!strcmp(words[i], "exit")) 
                exit=1;
            
            else{ 
                if(words[parallel]==NULL) SHELL_ERROR;
                else first_fail = 1;
                break;
            }
        }

        for(int i=parallel+1; words[i-1]!=NULL && words[i]!=NULL ;i++){
            if(!strcmp(words[i], "checkcpupercentage")){
                int status = 0;
                pid_t pid;
                
                if(words[i+1]==NULL || !strcmp(words[i+1], ";")){
                    printf("ERROR: Missing arguments in second command\n");
                    break;
                }

                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    checkcpupercentage(words[i+1]);
                }
                wait(&status);
                i++;
            }

            else if(!strcmp(words[i], "checkresidentmemory")){
                int status = 0;
                pid_t pid;
                
                if(words[i+1]==NULL || !strcmp(words[i+1], ";")){
                    printf("ERROR: Missing arguments in second command\n");
                    break;
                }
                
                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    checkresidentmemory(words[i+1]);
                }
                wait(&status);
                i++;
            }

            else if(!strcmp(words[i], "listFiles")){
                
                int status;
                pid_t pid;
                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    listFiles();
                }
                wait(&status);
            }

            else if(!strcmp(words[i], "sortFile")){
                int status = 0;
                pid_t pid;
                
                if(words[i+1]==NULL || !strcmp(words[i+1], ";")){
                    printf("ERROR: Missing arguments in second command\n");
                    break;
                }
                
                if ((pid = fork()) < 0)
                    FORK_ERROR;
                else if (pid == 0){
                    sortFile(words[i+1]);
                }
                wait(&status);
                i++;
            }

            else if(!strcmp(words[i], "listfiles")||!strcmp(words[i], "list_files")||!strcmp(words[i], "listfile")||!strcmp(words[i], "listFile")||!strcmp(words[i], "list")||!strcmp(words[i], "ls")){
                printf("ERROR: No such command exists. Did you mean \"listFiles\"?\n");        
            }

            else if(!strcmp(words[i], "sort")||!strcmp(words[i], "sortfiles")||!strcmp(words[i], "sortfile")){
                printf("ERROR: No such command exists. Did you mean \"sortFile\"?\n");  
                i++;      
            }

            else if(!strcmp(words[i], "exit")) 
                exit=1;
            
            else{ 
                if(first_fail == 1) printf("ERROR: Both commands are Illegal\n");
                else printf("ERROR: Second command is Illegal\n");
                break;
            }
            if(first_fail) printf("ERROR: First command is Illegal\n");
        }
        
        while ((wpid = wait(&status_t)) > 0);
        if(exit) break;    
    }
    return 0;
}

char **tokenize(char *input){

    int count = 0;
    char **tokens = (char **) malloc(32 * sizeof(char *));
    char *sch;
    sch = strtok(input, " \t\n"); 

    while (sch != NULL)
    {
        tokens[count] = sch;
        count++;
        sch = strtok(NULL, " \t\n"); 
    }

    free(sch);
    tokens[count] = NULL ;
    return tokens;
}