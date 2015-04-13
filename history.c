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

#include "history.h"

#include "prompt.h"
#include "config.h"
#include "util.h"
#include "string.h"

#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>

void history()
{
    char** h_base = history_base;
    int h_cur = history_cursor;
    int c = 0;
    for(int i=0;i<MAX_HISTORY_SIZE; i++, h_cur = mod(h_cur+1, MAX_HISTORY_SIZE))
    {
        char** history = h_base + h_cur;
        if((*history) == NULL) continue;
        
        //max unsigned integer = 4294967295 =  10 digits
        //+1 for null terminated string = 11 characters max
        char* ia = (char*)malloc(sizeof(char)*11);
        memset(ia, '\0', 11*sizeof(char));
        itoa(c,ia);
        
        write(STDOUT_FILENO, ia, strlen(ia));
        write(STDOUT_FILENO, " ", 1);
        write(STDOUT_FILENO, *history, strlen(*history));
        write(STDOUT_FILENO, "\n", 1);
        
        free(ia);
        c++;
    }
    exit(0);
}