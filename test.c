#include<unistd.h>

int main()
{
    char c;
    read(STDIN_FILENO, &c, 1);
    write(STDOUT_FILENO, "The file is changed.\n",6); 
    return 0;
}
