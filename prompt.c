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

#include "prompt.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

#include "config.h"
#include "util.h"

int history_user = 0;

int prompt(char* line)
{
    write(STDOUT_FILENO, prompt_str, strlen(prompt_str)*sizeof(char));
    
    char* initline = line;
    
    bool read = 1;
    
    while(1)
    {
        //printf("strlen: %d\n", strlen(initline));
        char c = read_char();
        switch(c)
        {
            case 0x04: { return 0; } // C-d exit
            case 0x0A:
            {
                write(STDOUT_FILENO, &c, 1);
                line = initline;
                history_user = history_cursor;
                
                if(strlen(line) == 0)
                    return 2;
                return 1; // Return key
            }
            case 0x1B:
            {
                c = read_char();
                
                if(c == '[')
                {
                    //                    printf("bracket detected!\n");
                    c = read_char();
                    
                    switch(c)
                    {
                        case 'A':
                        {
                            char* prev_cmd = *(history_base + history_user);
                            
                            
                            //Need to account for when it is full.
                            if(initline!=NULL && prev_cmd!=NULL) //some prev history and typed something
                                clear_chars(strlen(initline));
                            if(prev_cmd == NULL){ //do nothing
                                line = initline;
                                continue;
                            }
                            strcpy(initline, prev_cmd);
                            
                            //only changes cursor if previous history is not empty
                            if(*(history_base + mod(history_user - 1, MAX_HISTORY_SIZE)) != NULL)
                                history_user = mod((history_user-1), MAX_HISTORY_SIZE);
                            line = initline+strlen(initline);
                            write(STDOUT_FILENO, prev_cmd, MAX_LINE_SIZE);
                            continue;
                        }
                        case 'B':
                        {
                            char* next_cmd = *(history_base + mod(history_user + 1, MAX_HISTORY_SIZE));
                            
                            if(initline!=NULL) //some next history and typed something
                                clear_chars(strlen(initline));
                            if(next_cmd == NULL){ //next is empty
                                memset(initline,'\0',MAX_LINE_SIZE); //clear line buffer
                                line = initline;
                                continue;
                            }
                            
                            //only increments cursor if next command is not null
                            history_user= mod((history_user+1), MAX_HISTORY_SIZE);
                            strcpy(initline, next_cmd);
                            line = initline+strlen(initline);
                            write(STDOUT_FILENO, next_cmd, MAX_LINE_SIZE);
                            continue;
                        }
                    }
                }
                continue;
            }
            case 0x7F: case '\b':
            {
                if(strlen(initline) <= 0)
                    write(STDOUT_FILENO, &ALERT, 1);
                else
                {
                    write(STDOUT_FILENO, "\b \b", 3);
                    (*(--line)) = 0;
                }
                break;
            }
            default:
            {
                *line = c;
                write(STDOUT_FILENO, &c, 1);
                line++;
            }
        }
    }
}

void init_prompt()
{
    prompt_str = (char*)malloc(MAX_LINE_SIZE*sizeof(char));
    memset(prompt_str, '\0', MAX_LINE_SIZE);
    getcwd(prompt_str, MAX_LINE_SIZE);
    if(strlen(prompt_str) > 16)
    {
        
        char* prompt_str_base = prompt_str;
        strtok(prompt_str, "/");
        char* fileptr;
        while(prompt_str != NULL)
        {
            fileptr = prompt_str;
            prompt_str = strtok(NULL, "/");
        }
        char rs[MAX_LINE_SIZE];
        memset(rs, '\0', MAX_LINE_SIZE);
        strcat(rs, "/../");
        strcat(rs, fileptr);
        
        prompt_str = prompt_str_base;
        memset(prompt_str, '\0', MAX_LINE_SIZE);
        strcpy(prompt_str, rs);
        
    }
    strcat(prompt_str, " > ");
}

void init_history_table()
{
    history_base = (char**) malloc(MAX_HISTORY_SIZE*sizeof(char*)); // 10 registries
                                   
    for(int i=0; i< MAX_HISTORY_SIZE; i++)
    {
        (*(history_base+i)) = NULL;
    }
    history_cursor = 0;
}

void update_history(char* line)
{
    *(history_base + history_cursor) = line;
    history_cursor = mod(history_cursor + 1, MAX_HISTORY_SIZE);
    char* dep_ptr = *(history_base + history_cursor);
    if (dep_ptr != NULL)
        free(dep_ptr);
}