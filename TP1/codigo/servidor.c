#include "include/servidor.h"



int main(int argc, char *argv[])
{
	int sockfd, newsockfd, servlen, clilen, pid;
	struct sockaddr_un cli_addr, serv_addr;
	static int flag_user_ready_log;
	flag_user_ready_log = 0;
	
	/* Cargo mis dos usuarios*/
	cargo_usuarios();
	strcpy(arg0, argv[0]);
	strcpy(arg1, argv[1]);

	/* Se toma el nombre del socket de la línea de comandos */
	if (argc != 2)
	{
		printf("Uso: %s <nombre_de_socket>\n", argv[0]);
		exit(1);
	}

	else if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		perror("Error en creación de  socket");
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
		perror("Error en ligadura");
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
			perror("Error en accept sockfd");
			exit(1);
		}
		

		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			exit(1);
		}

		if (pid == 0)
		{ // proceso hijo

			close(sockfd);

			while (1)
			{

				//Antes de poder mandar cualquier comando, tengo que autenticarme.
				while (flag_user_ready_log == 0)
				{
					flag_user_ready_log = autenticacion();
				}

				if (flag_user_ready_log == 1)
				{
					printf(prompt_user_log);
					fflush(stdout);
					obtener_funcion(newsockfd);
				}
			}
		}
		else
		{
			close(newsockfd);
		}
	}
	return 0;
}

