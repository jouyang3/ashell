#include "exec.h"

#include "config.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int execute(char* line)
{
    char** s_argv;
    char*** p_argv;
    int s_argc = 0;
    int p_cmdc = 0;
    arg_parse(line, &s_argc, &s_argv, &p_cmdc, &p_argv);
    free(line);

    int status = 0;
    
//    printf("Examining argument list.\n");
//    for(int i=0; i<s_argc; i++)
//    {
//        printf("%s ", s_argv[i]);
//    }
//    printf("\n");
    
    
//    printf("Examining command directory.\n");
//    printf("# commands: %d.\n", p_cmdc);
//    for(int i=0; i<p_cmdc; ++i)
//    {
//        printf("command %d: ", i);
//        char** arg_ptr = p_argv[i];
//        while((*arg_ptr)!=NULL)
//        {
//            printf("%s ", *arg_ptr);
//            arg_ptr++;
//        }
//        printf("\n");
//    }
    
    
    //The following code is just a demo for a single-piped line
    int fds[2];
    pipe(fds);
    //execute the piping routine
    pid_t pid_src = fork();
    pid_t pid_dest = fork();
        
    if(pid_src == 0)
    {
        dup2(fds[1], STDOUT_FILENO); // need to read from stdin
        close(fds[0]);
                
        execvp(p_argv[0][0], p_argv[0]);
        
        perror(p_argv[0][0]);
    }
    
    if(pid_dest == 0)
    {
        dup2(fds[0], STDIN_FILENO);
        close(fds[1]);
                
        execvp(p_argv[1][0], p_argv[1]);
        perror(p_argv[1][0]);
    }
    
    close(fds[0]); close(fds[1]);
    
    if(pid_dest != 0){
        int status = 0;
        waitpid(pid_dest, &status, 0); //only wait on the very last child
    }
    
    //deallocate argument list
    for(int i=0; i<s_argc; i++)
        free(*(s_argv+i));
    free(s_argv);
    
    //deallocate command pointer
    free(p_argv);
    
    return status;
}

void arg_parse(char* line, int* argc, char*** argv, int* cmdc, char**** argp)
{
    *argv = (char**) malloc(sizeof(char*) * MAX_ARG_COUNT);
    
    //one can only have at maximum number of piped command equal to number of arguments
    *argp = (char***) malloc(sizeof(char**) * MAX_ARG_COUNT);
    
    for(int i=0;i<MAX_ARG_COUNT; i++)
    {
        (*argv)[i] = (char*)NULL;
        (*argp)[i] = (char**)NULL;
    }
    
    int ctr = 0; // command character counter
    char** cmd_base = *argv; // command base character counter
    int pctr = 0; // pipe counter
    bool allocated = false; // indicates if current argument has allocated memory
    
    *argc = -1;
    
    int cmd_argc = 0; //current command's argument counter, will be reseted per command
    
    bool done = false;
    while(!done)
    {
//        printf("Examining c: %c.\n",*line);
        switch(*line)
        {
            case '|':
            {
                *((*argp)+pctr) = cmd_base;
                
                cmd_base += cmd_argc+1; //skip over the pipe entry
                
                cmd_argc = 0;
                
                (*argv)[(++*argc)] = NULL;
                (*cmdc)++;
                ctr = 0;
                allocated = false;
                pctr++;
                break;
            }
            case '<': case '>':
            {
                (*argc)++;
                cmd_argc++;
                (*argv)[*argc] = (char*) malloc(MAX_ARG_SIZE * sizeof(char));
                (*argv)[*argc][ctr] = *line;
                ctr = 0;
                allocated = false;
                break;
            }
            case ' ':
            {
                ctr = 0;
                allocated = false;
                break;
            }
            case '\0':
            {
                *((*argp)+pctr) = cmd_base;
                (*cmdc)++;
                cmd_argc++;
                done = true;
                break;
            }
            default:
            {
                if(!allocated)
                {
                    (*argc)++;
                    cmd_argc++;
                    allocated = true;
                    
                    (*argv)[*argc] = (char*) malloc(MAX_ARG_SIZE * sizeof(char));
                }
//                printf("Adding %c at argv[%d][%d]\n", *line, *argc, ctr);
                (*argv)[*argc][ctr++] = *line;
                break;
            }
        }
        line++;
    }
    (*argc)++;
}
