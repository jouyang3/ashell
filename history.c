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