#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define TAM 80


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


struct Users
{
	char id[20];
	char pass[20];
};

struct Users arregloUsuarios[2];

int autenticacion(int socket);

int main(int argc, char *argv[])
{

	int sockfd, newsockfd, servlen, clilen, n, pid;
	int maquina_estado;
	struct sockaddr_un cli_addr, serv_addr;
	char buffer[TAM];

	struct Users usuario1, usuario2;

	strcpy(usuario1.id, "mati");
	strcpy(usuario1.pass, "ragnar");
	strcpy(usuario2.id, "agus");
	strcpy(usuario2.pass, "anij");
	arregloUsuarios[0] = usuario1;
	arregloUsuarios[1] = usuario2;

	/* Se toma el nombre del socket de la línea de comandos */
	if (argc != 2)
	{
		printf("Uso: %s <nombre_de_socket>\n", argv[0]);
		exit(1);
	}

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		perror("creación de  socket");
		exit(1);
	}

	/* Remover el nombre de archivo si existe */
	unlink(argv[1]);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, argv[1]);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

	if (bind(sockfd, (struct sockaddr *)&serv_addr, servlen) < 0)
	{
		perror("ligadura");
		exit(1);
	}

	printf("Proceso: %d - socket disponible: %s\n", getpid(), serv_addr.sun_path);

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
		if (newsockfd < 0)
		{
			perror("accept");
			exit(1);
		}

		pid = fork();
		if (pid < 0)
		{
			perror("ERROR en fork");
			exit(1);
		}

		if (pid == 0)
		{
			// proceso hijo
			close(sockfd);
			while (1)
			{
				memset(buffer, 0, TAM);
				maquina_estado = autenticacion(newsockfd);

				if (maquina_estado == 1)
				{
					printf("YUPIIIIIIIIIIII");
					n = write(newsockfd, "SI", 3);
					if (n < 0)
					{
						perror("escritura en socket");
						exit(1);
					}
				}
				else if (maquina_estado == 0)
				{
					printf("NOP");
					exit(0);
				}

				printf("Recibí: %s \n", buffer);

				n = write(newsockfd, "Obtuve su mensaje", 18);
				if (n < 0)
				{
					perror("escritura en socket");
					exit(1);
				}

				// Verificación de si hay que terminar
				if (!strcmp("fin", buffer))
				{
					printf("PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid());
					exit(0);
				}

				fflush(stdout);
				strcpy(buffer, "");
			}
		}
		else
		{
			printf("SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid);
			close(newsockfd);
		}
	}
	return 0;
}

int autenticacion(int socket)
{
	char autenticacion_id[TAM];
	char autenticacion_pass[TAM];
	char buffer[TAM];
	int n, comparo_id_usuario, comparo_pass_usuario, contador_intentos;

	contador_intentos = 1;

	while (contador_intentos <= 4)
	{
		memset(buffer, '\0', TAM);
		n = read(socket, buffer, TAM);
		if (n < 0)
		{
			perror("lectura de socket");
			exit(1);
		}

		strtok(buffer, "\n");
		strcpy(autenticacion_id, buffer);

		n = write(socket, "Obtuve su mensaje", 18);
		if (n < 0)
		{
			perror("escritura en socket");
			exit(1);
		}

		memset(buffer, '\0', TAM);
		n = read(socket, buffer, TAM);
		if (n < 0)
		{
			perror("lectura de socket");
			exit(1);
		}
		strtok(buffer, "\n");
		strcpy(autenticacion_pass, buffer);

		for (int i = 0; i < sizeof(arregloUsuarios); i++)
		{
			comparo_id_usuario = strcmp(arregloUsuarios[i].id, autenticacion_id);
			if (comparo_id_usuario == 0)
			{
				comparo_pass_usuario = strcmp(arregloUsuarios[i].pass, autenticacion_pass);
				if (comparo_pass_usuario == 0)
				{
					return 1;
				}
			}
		}

		contador_intentos++;

		if (contador_intentos == 4)
		{
			n = write(socket, "FIN", strlen("FIN"));
			if (n < 0)
			{
				perror("escritura en socket");
				exit(0);
			}
		}
		else
		{
			n = write(socket, "Obtuve su mensaje", 18);
			if (n < 0)
			{
				perror("escritura en socket");
				exit(1);
			}
		}
	}
	return 0;
}
