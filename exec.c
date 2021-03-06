/**
 * Copyright (C) 2015  Jun D. Ouyang, Isaac Baron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "exec.h"

#include "config.h"
#include "util.h"
#include "history.h"
#include "ls.h"
#include "cd.h"
#include "pwd.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

//change stuff here.
void exec(char** argv)
{
    if(strcmp(argv[0],"history") == 0)
    {
        history();
    } else if(strcmp(argv[0], "ls") == 0){
        ls(argv[1]);
    } else if(strcmp(argv[0], "pwd") == 0)
    {
        pwd();
    } else
    {
        execvp(argv[0], argv);
    }
}


int execute(char* line)
{
    char** s_argv;
    char*** p_argv;
    int s_argc = 0;
    int p_cmdc = 0;
    arg_parse(line, &s_argc, &s_argv, &p_cmdc, &p_argv);
    free(line);
    
    //need to premade p_cmdc-1 pipes
    int** pipe_tb = (int**) malloc(sizeof(int*)*(p_cmdc-1));
    
//        printf("p_cmdc = %d\n", p_cmdc);
    for(int i=0;i<p_cmdc-1; ++i)
    {
        int* fd = (int*) malloc(2*sizeof(int));
        pipe(fd);
        pipe_tb[i] = fd;
    }
    
    //base case, 1 command, no pipe
    int status = 0;
    
    if(p_argv[0][0] == NULL)
    {
        status = 0;
    }
    else if(strcmp(p_argv[0][0], "exit") == 0)
    {
        exit(0); // GRACEFULLY!!!!
    }
    else if(strcmp(p_argv[0][0], "cd") == 0) // run in parent
    {
        status = cd(p_argv[0][1]);
    }
    else if(p_cmdc == 1)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            int fd = redirect(p_argv[0]);
            close(fd);
            
            char* cmd = p_argv[0][0];
            
            //            printf("cmd: %shaha\n",cmd);
            //            printf("strcmp(cmd, \"history\") = %d\n",strcmp(cmd,"history"));
            
            exec(p_argv[0]);
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
                    int rfd = redirect(p_argv[i]);
                    close(rfd);
                    
                    close_pipes(num_pipe, pipe_tb);
                } else
                {
                    dup2(pipe_tb[i-1][0], STDIN_FILENO);
                    dup2(pipe_tb[i][1], STDOUT_FILENO);
                    close_pipes(num_pipe, pipe_tb);
                }
                
                //insert stuffs here
                exec(p_argv[i]);
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
//            printf("waiting on %d\n",pids[i]);
            waitpid(pids[i], &status, 0);
        }
        
    }


    //deallocate argument list
    for(int i=0; i<s_argc; i++)
        free(*(s_argv+i));
    free(s_argv);
    
    //deallocate command pointer
    free(p_argv);
    
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
            
            chmod(out_file, mode & ~mask);
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
