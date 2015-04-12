#include "exec.h"

#include "config.h"
#include "util.h"
#include "history.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

int execute(char* line)
{
    char** s_argv;
    char*** p_argv;
    int s_argc = 0;
    int p_cmdc = 0;
    arg_parse(line, &s_argc, &s_argv, &p_cmdc, &p_argv);
    free(line);
    
    int status = exec(p_cmdc, p_argv);

    //deallocate argument list
    for(int i=0; i<s_argc; i++)
        free(*(s_argv+i));
    free(s_argv);
    
    //deallocate command pointer
    free(p_argv);
    
    return status;
}

int exec(int p_cmdc, char*** p_argv)
{
    
    //need to premade p_cmdc-1 pipes
    int** pipe_tb = (int**) malloc(sizeof(int*)*(p_cmdc-1));
    
    //    printf("p_cmdc = %d\n", p_cmdc);
    for(int i=0;i<p_cmdc-1; ++i)
    {
        int* fd = (int*) malloc(2*sizeof(int));
        pipe(fd);
        pipe_tb[i] = fd;
    }
    
    //base case, 1 command, no pipe
    int status = 0;
    
    if(p_cmdc == 1)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            int fd = redirect(p_argv[0]);
            close(fd);
            
            char* cmd = p_argv[0][0];
            
//            printf("cmd: %shaha\n",cmd);
//            printf("strcmp(cmd, \"history\") = %d\n",strcmp(cmd,"history"));
            
            if(strcmp(cmd,"history") == 0)
            {
                history();
            } else
            {
                execvp(p_argv[0][0], p_argv[0]);
            }
            print_err(p_argv[0][0]);
        }
        waitpid(pid, &status, 0);
        return status;
    }
    else { // 2 or above commands
        
        int num_pipe = p_cmdc - 1;
        //first command need pipe only to stdout
        pid_t pids[p_cmdc];
        
        for(int i=0; i<p_cmdc; i++)
        {
            pids[i] = fork();
            
            if(pids[i] == 0)
            {
                if(i == 0)
                {
                    dup2(pipe_tb[0][1], STDOUT_FILENO);
                    
                    //redirection only occurs at first command
                    int rfd = redirect(p_argv[0]);
                    close(rfd);
                    close_pipes(num_pipe, pipe_tb);
                } else if (i == p_cmdc - 1)
                {
                    dup2(pipe_tb[i-1][0], STDIN_FILENO);
                    
                    //or last command
                    int rfd = redirect(p_argv[0]);
                    close(rfd);
                    
                    close_pipes(num_pipe, pipe_tb);
                } else
                {
                    dup2(pipe_tb[i-1][0], STDIN_FILENO);
                    dup2(pipe_tb[i][1], STDOUT_FILENO);
                    close_pipes(num_pipe, pipe_tb);
                }
                
                execvp(p_argv[i][0], p_argv[i]);
                print_err(p_argv[i][0]);
            }
        }
        
        for(int j=0; j<num_pipe; ++j)
        {
            close(pipe_tb[j][0]);
            close(pipe_tb[j][1]);
        }
        
        for(int i=0; i<p_cmdc; i++)
        {
                        printf("waiting on %d\n",pids[i]);
            waitpid(pids[i], &status, 0);
        }
        
    }
    return status;
}

void print_err(char* cmd)
{
    if(errno == ENOENT)
    {
        char* msg = "No such file or directory\n";
        write(STDERR_FILENO, cmd, strlen(cmd));
        write(STDERR_FILENO, ": ", 2);
        write(STDERR_FILENO, msg, strlen(msg));
    }
}

void close_pipes(int num_pipe, int** pipe_tb)
{
    for(int j=0; j<num_pipe; ++j)
    {
        close(pipe_tb[j][0]);
        close(pipe_tb[j][1]);
    }
}

int redirect(char** argv)
{
    int i = 0;
    while(argv[i] != NULL)
    {
//        printf("argv[%d] = %s\n", i, argv[i]);
        if(argv[i][0] == '<')
        {
            argv[i] = NULL;
            char* in_file = argv[i + 1];
            printf("input file name: %s\n", in_file);
            int fd = open(in_file, O_RDONLY);
            dup2(fd, STDIN_FILENO);
            return fd;
        } else if(argv[i][0] == '>')
        {
            argv[i] = NULL;
            char* out_file = argv[i + 1];
//            printf("output file name: %s\n", out_file);
            
            //obtain current user's umask
            mode_t mask = umask(S_IRUSR);
            umask(mask);
            
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
            
            
            int fd = open(
                out_file,
                O_WRONLY | O_CREAT, mode & ~mask);
            
            fchmod(fd, mode & ~mask);
//            printf("errno: %s\n",strerror(errno));
//            printf("EACCES: %d\n",EACCES);
//            printf("output file fd: %d\n", fd);
            dup2(fd, STDOUT_FILENO);
            return fd;
        }
        i++;
    }
    return -1;
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
                //clearout the string buffer
                memset((*argv)[*argc], '\0', MAX_ARG_SIZE * sizeof(char));
                
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
                    //clearout the string buffer
                    memset((*argv)[*argc], '\0', MAX_ARG_SIZE * sizeof(char));
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
