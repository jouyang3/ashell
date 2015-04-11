#include "util.h"

#include <unistd.h>


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