#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>

#define TAM 512
void status_sat_read();

struct Satelite
{
	char id[TAM];
	char uptime[TAM];
	char version[TAM];
	char consumo[TAM];
};

static struct Satelite sat;
struct timeval time1, time2;

int main(int argc, char *argv[])
{
	gettimeofday(&time1, NULL);
	sleep(3);
	status_sat_read();
printf("Content-type: text/html\n\n");
 printf("<html><title>Hello</title><body>\n");
 printf("Goodbye Cruel World\n");
 printf("</body></html>");
	printf("%s \n",sat.uptime);
	printf("%s \n",sat.consumo);
}

void status_sat_read()
{
	FILE *top_file;
	char aux[TAM] = {0};
	char aux_pid[TAM] = {0};

	bzero(sat.uptime, sizeof(sat.uptime));
	bzero(sat.consumo, sizeof(sat.consumo));
	bzero(sat.version, sizeof(sat.version));

	gettimeofday(&time2, NULL);

	sprintf(aux, "%0.2f", (double)(time2.tv_usec - time1.tv_usec) / 1000000 +
			   (double)(time2.tv_sec - time1.tv_sec));

	strcpy(sat.uptime, aux);

	bzero(aux, sizeof(aux));
	strcpy(aux, "ps -Ao vsize,pid,pcpu | grep ");
	sprintf(aux_pid, "%ld", (long)getpid());
	strcat(aux, aux_pid);
	strcat(aux, " >> ./clienteBIN/top.info");

	system("rm ./clienteBIN/top.info");
	system(aux);
	top_file = fopen("./clienteBIN/top.info", "r");
	bzero(aux, sizeof(aux));
	fread(aux, 1, sizeof(aux) - 1, top_file);
	strtok(aux, "\n");
	strcpy(sat.consumo, aux);
	fclose(top_file);
}
