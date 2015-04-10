#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdio.h>
#include<termios.h>
static const char* SHELL_DELIMITER = " ";
static const int MAX_LINE_SIZE = 100000;
static const int MAX_ARG_SIZE = 1000;
static const int MAX_ARG_COUNT = MAX_LINE_SIZE/MAX_ARG_SIZE;
static const int MAX_HISTORY_SIZE = 10;
static const char LINE_BREAK = 0x0A;
static const char ALERT = '\a';


int prompt(char*);
char read_char();
void update_history(char*);
void init_history_table();
void clear_chars(int);
int mod(int,int);
void arg_parse(char*, int*, char***);
void ResetCanonicalMode(int fd, struct termios *savedattributes);
void SetNonCanonicalMode(int fd, struct termios *savedattributes);

char** history_base;
int history_cursor = 0;
int history_user = 0;


int main(int argc, char* argv[])
{
    history_base = (char**) malloc(MAX_HISTORY_SIZE*sizeof(char*)); //10 registries
    
    init_history_table();
    
    struct termios SavedTermAttributes;
    SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
    
    while(true)
    {
        char* line = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
        memset((void*)line, '\0', MAX_LINE_SIZE);
        
        char* history_line = (char*) malloc(MAX_LINE_SIZE * sizeof(char));
        int status = prompt(line);
        
        strcpy(history_line, line); //strtok is destructive, need to preserve spaces
        if(!status) break; // normal exit of shell
        
        update_history(history_line);
        
        char** s_argv;
        int s_argc = 0;
        arg_parse(line, &s_argc, &s_argv);
        free(line);
        
//        for(int i=0;i<s_argc; i++)
//        {
//            printf("arg[%d] = %s\n",i,s_argv[i]);
//        }
        
        if(strcmp(s_argv[0], "exit") == 0) break;
        else{
            //int fds[2];
            //pipe(fds);
            pid_t pid = fork();
//            printf("pid = %d\n",pid);
            
            int status = 0;
            
            if(pid == 0)
            {
                
                execvp(*s_argv, s_argv);
                
            } else {
                waitpid(pid, &status, 0);
            }
        }
        
        //deallocate argument list
        for(int i=0; i<s_argc; i++)
            free(*(s_argv+i));
        free(s_argv);
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
    
    while(1)
    {
        //printf("strlen: %d\n", strlen(initline));
        char c = read_char();
        switch(c)
        {
            case 0x04: {printf("C-d.\n"); return 0;} // C-d exit
            case 0x0A:
            {
                write(STDOUT_FILENO, &c, 1);
                line = initline;
                history_user = history_cursor;
                return 1; // Return key
            }
            case 0x1B:
            {
                c = read_char();
                
                if(c == '[')
                {
//                    printf("bracket detected!\n");
                    c = read_char();
                    
                    switch(c)
                    {
                        case 'A':
                        {
                            char* prev_cmd = *(history_base + history_user);
                            
                            if(initline!=NULL && prev_cmd!=NULL) //some prev history and typed something
                                clear_chars(strlen(initline));
                            if(prev_cmd == NULL){ //do nothing
                                line = initline;
                                continue;
                            }
                            strcpy(initline, prev_cmd);
                            
                            //only changes cursor if previous history is not empty
                            if(*(history_base + mod(history_user - 1, MAX_HISTORY_SIZE)) != NULL)
                               history_user = mod((history_user-1), MAX_HISTORY_SIZE);
                            line = initline+strlen(initline);
                            write(STDOUT_FILENO, prev_cmd, MAX_LINE_SIZE);
                            continue;
                        }
                        case 'B':
                        {
                            char* next_cmd = *(history_base + mod(history_user + 1, MAX_HISTORY_SIZE));
                            
                            if(initline!=NULL) //some next history and typed something
                                clear_chars(strlen(initline));
                            if(next_cmd == NULL){ //next is empty
                                memset(initline,'\0',MAX_LINE_SIZE); //clear line buffer
                                line = initline;
                                continue;
                            }
                            
                            //only increments cursor if next command is not null
                            history_user= mod((history_user+1), MAX_HISTORY_SIZE);
                            strcpy(initline, next_cmd);
                            line = initline+strlen(initline);
                            write(STDOUT_FILENO, next_cmd, MAX_LINE_SIZE);
                            continue;
                        }
                    }
                }
                continue;
            }
            case 0x7F: case '\b':
            {
                if(strlen(initline) <= 0)
                   write(STDOUT_FILENO, &ALERT, 1);
                else
                {
                   write(STDOUT_FILENO, "\b \b", 3);
                    (*(--line)) = 0;
                }
                break;
            }
            default:
            {
                *line = c;
                write(STDOUT_FILENO, &c, 1);
                line++;
            }
        }
    }
}

void init_history_table()
{
    for(int i=0; i< MAX_HISTORY_SIZE; i++)
    {
        (*(history_base+i)) = NULL;
    }
}

int mod(int val, int n)
{
    int r = val % n;
    return r<0? r+n:r;
}

void clear_chars(int num_char)
{
    for(int i=0; i<num_char; i++){
        write(STDOUT_FILENO, "\b \b", 3);
    }
    
}

void update_history(char* line)
{
    *(history_base + history_cursor) = line;
    history_cursor = mod(history_cursor + 1, MAX_HISTORY_SIZE);
    char* dep_ptr = *(history_base + history_cursor);
    if (dep_ptr != NULL)
        free(dep_ptr);
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
//    printf("%s\n",line);
    *argv = (char**) malloc(sizeof(char*) * MAX_ARG_COUNT);
    for(int i=0;i<MAX_ARG_COUNT; i++)
    {
        (*argv)[i] = (char*)NULL;
    }
    
    char* ptr = strtok(line, SHELL_DELIMITER);
    while(ptr != NULL)
    {
        
//        printf("ptr = %s\n",ptr);
        (*argv)[(*argc)] = (char*) malloc(sizeof(char) * MAX_ARG_SIZE);
        strcpy((*argv)[(*argc)++], ptr);
        ptr = strtok(NULL, SHELL_DELIMITER);
    }
//    printf("arg_parse: argc: %d\n", *argc);
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