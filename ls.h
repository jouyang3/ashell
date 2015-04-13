#ifndef LS_H
#define LS_H

#include <sys/stat.h>

void ls(char*);

void print_permission(struct stat file_info);

#endif