#include<unistd.h>
#include<stdlib.h>
#include<cstring>
#include<stdio.h>
#include<termios.h>
static const char* SHELL_DELIMITER = " ";
static const int MAX_LINE_SIZE = 100000;
static const int MAX_ARG_SIZE = 1000;
static const int MAX_ARG_COUNT = MAX_LINE_SIZE/MAX_ARG_SIZE;
static const char LINE_BREAK = 0x0A;
static const char ALERT = '\a';


int prompt(char*);
char read_char();
void arg_parse(char*, int*, char***);
void two_d_char_arr(int, int, char***);
void ResetCanonicalMode(int fd, struct termios *savedattributes);
void SetNonCanonicalMode(int fd, struct termios *savedattributes);

int main(int argc, char* argv[])
{
    struct termios SavedTermAttributes;
    SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
    
    while(true)
    {
        char* line = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
        int status = prompt(line);
        
        if(!status) break; // normal exit of shell
        
        char** s_argv;
        int* s_argc;
        arg_parse(line, s_argc, &s_argv);
        
        if(strcmp(s_argv[0], "exit") == 0) break;
        
    }
    
    ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
    
    return 0;
}

int prompt(char* line)
{
    char* prompt_str = "aShell >> ";
    write(STDOUT_FILENO, prompt_str, strlen(prompt_str)*sizeof(char));
    
    char* initline = line;
    
    bool read = 1;
    
    int cursor = 0;
    
    while(1)
    {
        //printf("strlen: %d\n", strlen(initline));
        char c = read_char();
        switch(c)
        {
            case 0x04: return 0; // C-d exit
            case 0x0A: write(STDOUT_FILENO, &c, 1); line = initline; return 1; // Return key
            case 0x1B:
//                printf("space detected!\n");
//                write(STDOUT_FILENO, &c, 1);
//                (*line++) = c;
                
                c = read_char();
//                write(STDOUT_FILENO, &c, 1);
//                (*line++) = c;
                
                if(c == '[')
                {
//                    printf("bracket detected!\n");
                    c = read_char();
                    
                    switch(c)
                    {
                        case 'A': break;
                        case 'B': break;
                        case 'C':
                            if(cursor >= strlen(initline))
                                write(STDOUT_FILENO, &ALERT, 1);
                            else
                            {
                                write(STDOUT_FILENO, initline+cursor, 1);
                                cursor++; line++;
                            }
                            break;
                        case 'D':
                            if(cursor <= 0)
                                write(STDOUT_FILENO, &ALERT, 1);
                            else
                            {
                                write(STDOUT_FILENO, "\b", 1);
                                cursor--;
                                line--;
                            }
                            break;
                    }
                }
                break;
            case 0x7F: case '\b':
                if(strlen(initline) <= 0)
                   write(STDOUT_FILENO, &ALERT, 1);
                else
                {
                   write(STDOUT_FILENO, "\b \b", 3);
                    (*(--line)) = 0; cursor--;
                }
                break;
            default: *line = c; write(STDOUT_FILENO, &c, 1); line++; cursor++;
        }
    }
}

char read_char()
{
    //char* RXChar = (char*) malloc(sizeof(char));
    char RXChar;
    read(STDIN_FILENO, &RXChar, 1);
    return RXChar;
}

void arg_parse(char* line, int* argc, char*** argv)
{
    two_d_char_arr(MAX_ARG_COUNT, MAX_ARG_SIZE, argv);
//    printf("%s\n",line);
    char* ptr = strtok(line, SHELL_DELIMITER);
    argc = (int*)malloc(sizeof(int));
    (*argc) = 0;
    while(ptr != NULL)
    {
        printf("ptr = %s\n",ptr);
        strcpy((*argv)[(*argc)++], ptr);
        ptr = strtok(NULL, SHELL_DELIMITER);
    }
    
}

void two_d_char_arr(int m, int n, char*** array)
{
    *array = (char**) malloc(m * sizeof(char*));
    
    for(int i = 0; i < m; i++)
    {
        (*array)[i] = (char*) malloc(n * sizeof(char));
    }
}

void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}

void SetNonCanonicalMode(int fd, struct termios *savedattributes){
    struct termios TermAttributes;
    char *name;
    
    // Make sure stdin is a terminal.
    if(!isatty(fd)){
        fprintf (stderr, "Not a terminal.\n");
        exit(0);
    }
    
    // Save the terminal attributes so we can restore them later.
    tcgetattr(fd, savedattributes);
    
    // Set the funny terminal modes.
    tcgetattr (fd, &TermAttributes);
    TermAttributes.c_lflag &= ~(ICANON | ECHO); // Clear ICANON and ECHO.
    TermAttributes.c_cc[VMIN] = 1;
    TermAttributes.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSAFLUSH, &TermAttributes);
}