#include "pwd.h"
#include "config.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void pwd()
{
    char wd[MAX_LINE_SIZE];
    getcwd(wd, MAX_LINE_SIZE);
    write(STDOUT_FILENO, wd, strlen(wd));
    write(STDOUT_FILENO, "\n", 1);
    exit(0);
}