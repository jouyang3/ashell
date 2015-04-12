#include "ls.h"

#includ<sys/types.h>
#include<dirent.h>
#includ<unistd.h>

void ls(char* directory)
{
	DIR* dirp;
	struct dirent* dir_entry;
	
	dirp = opendir(directory);
	
	do
	{
		dir_entry = readdir(dirp);
		dir_entry -> d_name;
		write(STDOUT_FILENO, d_name, 1000);
	}while(dir_entry != NULL);
	
	closedir(dirp);
}