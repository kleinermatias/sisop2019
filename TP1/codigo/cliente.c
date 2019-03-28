/* Cliente en el dominio Unix - orientado a corrientes */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define TAM 80

int loggin(int sockfd);

int main(int argc, char *argv[])
{
	int sockfd, servlen;
	struct sockaddr_un serv_addr;
	int terminar = 0;

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

	while (1)
	{
		int prueba;
		prueba = loggin(sockfd);

		if (prueba == 1) {
			printf("SEEEEEEEEEEEEEEEE");
		}
		if (terminar)
		{
			printf("Finalizando ejecución\n");
			exit(0);
		}
	}
	return 0;
}

int loggin(int sockfd)
{
	char buffer[TAM];
	int terminar = 0;
	int n;
	inicio: memset(buffer, '\0', TAM);
	printf(ANSI_COLOR_GREEN " USUARIO " ANSI_COLOR_RESET);
	fgets(buffer, TAM - 1, stdin);

	n = write(sockfd, buffer, strlen(buffer));
	if (n < 0)
	{
		perror("escritura de socket");
		exit(1);
	}

	// Verificando si se escribió: fin
	buffer[strlen(buffer) - 1] = '\0';
	if (!strcmp("fin", buffer))
	{
		terminar = 1;
	}

	memset(buffer, '\0', TAM);
	n = read(sockfd, buffer, TAM);
	if (n < 0)
	{
		perror("lectura de socket");
		exit(1);
	}

	printf("Respuesta: %s\n", buffer);
	if (strcmp(buffer, "FIN") == 0)
	{
		terminar = 1;
	}

	else if (strcmp(buffer, "SI") == 0)
	{
		return 1;
	}

	else {
		goto inicio;
	}



	if (terminar)
	{
		printf("Finalizando ejecución\n");
		exit(0);
	}

	return 0;
}
