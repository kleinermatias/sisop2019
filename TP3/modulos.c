#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

int main(int argc, char *argv[])
{

FILE *pipe;
    char buffer[100];

    pipe = popen("lsmod", "r");
    if (pipe == NULL)
        return -1;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        fprintf(stdout, "%s", buffer);
    pclose(pipe);


   printf("\n \n mensajito del modulo cargado es>");


    pipe = popen("dmesg", "r");
    if (pipe == NULL)
        return -1;  
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        fprintf(stdout, "%s", buffer);
    pclose(pipe);

   system("sudo dmesg -C");


return (0);

}
