#ifndef CONFIG_H
#define CONFIG_H

static const char SHELL_DELIMITER = ' ';
static const char PIPE_CHARACTER = '|';
static const char REDIRECT_IN_CHARACTER = '<';
static const char REDIRECT_OUT_CHARACTER = '>';
static const int MAX_LINE_SIZE = 100000;
static const int MAX_ARG_SIZE = 1000;
static const int MAX_ARG_COUNT = MAX_LINE_SIZE/MAX_ARG_SIZE;
static const int MAX_HISTORY_SIZE = 10;
static const char LINE_BREAK = 0x0A;
static const char ALERT = '\a';

#endif