#include<unistd.h>

int main()
{
    char c;
    read(STDIN_FILENO, &c, 1);
    write(STDOUT_FILENO, "HELLO\n",6); 
    return 0;
}
