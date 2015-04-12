#ifndef EXEC_H
#define EXEC_H

int execute(char*);
int exec(int, char***);
void arg_parse(char*, int*, char***, int*, char****);
void close_pipes(int, int**);
int redirect(char** argv);
void print_err(char*);

#endif