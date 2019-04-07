
#include "include/cliente.h"



int main(int argc, char *argv[])
{
	int sockfd, servlen;
	struct sockaddr_un serv_addr;
	

	/* Inicio referencia de mi var time1, para luego poder hacer la resta de time2-time1 y sacar asi el uptime.*/
	gettimeofday(&time1, NULL);

	if (argc < 2)
	{
		fprintf(stderr, "Uso %s archivo\n", argv[0]);
		exit(0);
	}

	memset((char *)&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, argv[1]);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		perror("creación de socket");
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0)
	{
		perror("conexión");
		exit(1);
	}


	/* Inicio estructura satelite.*/
	status_sat_read();

	while (1)
	{
		/* Siempre me quedo en este while para atender las peticiones del servidor.*/
		func_detect(sockfd);
	}
	return 0;
}


