#ifndef UTIL_H
#define UTIL_H

char read_char();
void clear_chars(int);
int mod(int,int);
char* itoa(int, char*);

char* fullpath(char*);

typedef int bool;
#define true 1
#define false 0

#endif