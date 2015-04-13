#include "ls.h"
#include "util.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

void p(char*);

void ls(char* directory)
{
    if(directory == NULL || strlen(directory) == 0)
    {
        directory = ".";
    }
    char* fulldir = fullpath(directory);
    
    DIR* dirp;
    struct dirent* dir_entry;
    
    dirp = opendir(fulldir);
    
    while ((dir_entry = readdir(dirp)) != NULL)
    {
        char* file_name = (*dir_entry).d_name;
        
        struct stat file_info;
        stat(file_name, &file_info);
        
        print_permission(file_info);
        
        write(STDOUT_FILENO, " ", 1);
        
        write(STDOUT_FILENO, file_name, strlen(file_name));
        
        write(STDOUT_FILENO, "\n",1);
        
        
    }
    
    closedir(dirp);
    exit(0);
}

void print_permission(struct stat file_info)
{
    mode_t perm = file_info.st_mode;
    
    if(S_ISREG(perm)) p("-"); else p("d");
    if((perm & S_IRUSR) != 0) p("r"); else p("-");
    if((perm & S_IWUSR) != 0) p("w"); else p("-");
    
    //Check if setuid
    if((perm & S_IXUSR) != 0) p("x"); else if((perm & S_ISUID) != 0) p("s"); else p("-");
    
    if((perm & S_IRGRP) != 0) p("r"); else p("-");
    if((perm & S_IWGRP) != 0) p("w"); else p("-");
    
    //Check if setgid
    if((perm & S_IXGRP) != 0) p("x"); else if((perm & S_ISGID) != 0) p("s"); else p("-");
    if((perm & S_IROTH) != 0) p("r"); else p("-");
    if((perm & S_IWOTH) != 0) p("w"); else p("-");
    
    //Check sticky
    if((perm & S_IXOTH) != 0) p("x"); else if((perm & S_ISVTX) != 0) p("t"); else p("-");
}

void p(char* s)
{
    write(STDOUT_FILENO, s, strlen(s));
}