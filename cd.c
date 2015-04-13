#include "cd.h"
#include "config.h"
#include "prompt.h"
#include "util.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>

int cd(char* path)
{
    char* fulldir = fullpath(path);
    
    int status = chdir(fullpath(path));
    
    init_prompt();
    
    free(fulldir);
    
    return status;
}