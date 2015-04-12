#include "cd.h"

#include <unistd.h>

void cd(char* path)
{
	chdir(path);
}