#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "canon.h"
#include "config.h"
#include "prompt.h"
#include "exec.h"


int main(int argc, char* argv[])
{

    init_history_table();
    
    SetNonCanon();
    
    while(true)
    {
        char* line = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
        memset((void*)line, '\0', MAX_LINE_SIZE);
        
        char* history_line = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
        int status = prompt(line);
        
        strcpy(history_line, line); //strtok is destructive, need to preserve spaces
        if(!status) break; // normal exit of shell
        
        update_history(history_line);
        
        execute(line);
    }
    
    ResetCanon();
    
    return 0;
}