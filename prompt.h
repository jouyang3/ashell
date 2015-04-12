#ifndef PROMPT_H
#define PROMPT_H

char* prompt_str;

char** history_base;
int history_cursor;

int prompt(char*);
char read_char();
void init_history_table();
void update_history(char*);

#endif