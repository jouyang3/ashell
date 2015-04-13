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

#include "util.h"
#include "config.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//#include <stdio.h>


int mod(int val, int n)
{
    int r = val % n;
    return r<0? r+n:r;
}

void clear_chars(int num_char)
{
    for(int i=0; i<num_char; i++){
        write(STDOUT_FILENO, "\b \b", 3);
    }
    
}

char read_char()
{
    //char* RXChar = (char*) malloc(sizeof(char));
    char RXChar;
    read(STDIN_FILENO, &RXChar, 1);
    return RXChar;
}

char* itoa(int num, char* rs)
{
    char C[10] = {'0','1','2','3','4','5','6','7','8','9'};
    
    
    char temp[11];
    memset(temp, '\0', 11*sizeof(char));
    int r = -1;
    int q = num;
    
    for(int i=0; i<11; i++){
        rs[i] = '\0';
    }
    
    int i=10;
    
    if(q==0)
        temp[10] = '0';
    else
        while(q != 0)
        {
            r = q%10;
            temp[i--] = C[r];
            q /= 10;
        }
    
    //flip the array
    i = 10;
    for(int j=0; j<11; j++){
        rs[j] = temp[i--];
    }
    
//    printf("rs = %s\n", rs);
    return rs;
}

char* fullpath(char* path)
{
    char* home_env = getenv("HOME");
    
    if(path == NULL || strlen(path) == 0)
    {
        return home_env;
    } else if(path[0] == '/')
    {
        return path;
    } else if(path[0] == '.' && path[1] == '.')
    {
        char* fileptr = path;
        
        fileptr = fileptr + 2;
        
        char* full_path = (char*) malloc(MAX_LINE_SIZE*sizeof(char));
        memset(full_path, '\0', MAX_LINE_SIZE);
        getcwd(full_path, MAX_LINE_SIZE);
        
        for(int i = strlen(full_path) - 1; i > -1; i--)
        {
            if(full_path[i] == '/')
            {
                full_path[i] = '\0';
                break;
            }
            full_path[i] = '\0';
        }
        strcat(full_path, fileptr);

        return full_path;
    } else
    {
        char* full_path = (char*) malloc(MAX_LINE_SIZE*sizeof(char));
        memset(full_path, '\0', MAX_LINE_SIZE);
        getcwd(full_path, MAX_LINE_SIZE);
        
        if(path[0] == '.')
        {
            path += 1;
        } else
        {
            strcat(full_path, "/");
        }
        
        strcat(full_path, path);
        return full_path;
    }

}