#include "exec.h"

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int execute(char* line)
{
    char** s_argv;
    int s_argc = 0;
    arg_parse(line, &s_argc, &s_argv);
    free(line);

    int status = 0;
    
    if(strcmp(s_argv[0], "exit") == 0)
        status = 0;
    else
    {
        //int fds[2];
        //pipe(fds);
        pid_t pid = fork();
        //            printf("pid = %d\n",pid);
        
        if(pid == 0)
        {
            
            execvp(*s_argv, s_argv);
            
        }
        else
        {
            waitpid(pid, &status, 0);
        }
    }
    
    //deallocate argument list
    for(int i=0; i<s_argc; i++)
        free(*(s_argv+i));
    free(s_argv);
    
    return status;
}

void arg_parse(char* line, int* argc, char*** argv)
{
    //    printf("%s\n",line);
    *argv = (char**) malloc(sizeof(char*) * MAX_ARG_COUNT);
    for(int i=0;i<MAX_ARG_COUNT; i++)
    {
        (*argv)[i] = (char*)NULL;
    }
    
    char* ptr = strtok(line, SHELL_DELIMITER);
    while(ptr != NULL)
    {
        
        //        printf("ptr = %s\n",ptr);
        (*argv)[(*argc)] = (char*) malloc(sizeof(char) * MAX_ARG_SIZE);
        strcpy((*argv)[(*argc)++], ptr);
        ptr = strtok(NULL, SHELL_DELIMITER);
    }
    //    printf("arg_parse: argc: %d\n", *argc);
}
