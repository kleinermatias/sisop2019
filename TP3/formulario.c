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
        char anio[100]={0};
        char dia[100]={0};

        strcat(anio,inputParse);
        strcat(dia,inputParse);

        char *year=strtok(dia,"&");
        char *day=strtok(NULL," ");
        day=strtok(day,"=");
        day=strtok(NULL," ");

        char path[100]={0};
        strcpy(path,"aws s3 --no-sign-request ls --recursive noaa-goes16/ABI-L2-CMIPF/");
        strcat(path,year);
        strcat(path,"/");
	if(strlen(day)==0){
                printf("Error, pone algo mayor a 0 cheeee \n");
		return 0;
        }

        else if(strlen(day)==1){
		char tmp[50];
		strcpy(tmp,  "00");
		strcat(tmp, day);
		strcpy(day, tmp);
		printf("%s \n",day);
	}

	else if(strlen(day)==2){
                char tmp[50];
                strcpy(tmp,  "0");
                strcat(tmp, day);
                strcpy(day, tmp);
                printf("%s \n",day);
        }

	else if(strlen(day)>3){
		printf("te fuiste al asco \n");
		return 0;
        }

	int num = atoi(day);
	if(num>365){
                printf("te fuiste al asco x2\n");
                return 0;
        }

        strcat(path,day);
        strcat(path,"/");
        strcat(path," | grep M3C13");
	//printf(path);
	system(path);
        printf(" - - - - -  ");
        return 0;
}
