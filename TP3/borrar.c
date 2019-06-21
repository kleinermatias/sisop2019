#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/sysinfo.h>


int main(int argc, char* argv[]) 
{

	char* readInput = strchr(getenv("QUERY_STRING"),'=')+1;

        char *inputParse = strdup(readInput);
        char *name=strtok(inputParse,"&");

   printf("el nombre del modulo a borrar es %s", name);
   system("sudo dmesg -C");


	char path[100]={0};
        strcpy(path,"sudo rmmod ");
	strcat(path,name);

if(system(path)!=0){
printf(" PEEEEEERO, no esta cargado ese modulo :c");
}

}
