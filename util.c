#include "util.h"

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