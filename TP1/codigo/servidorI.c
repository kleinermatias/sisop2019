#include "include/servidorI.h"



int main(int argc, char *argv[])
{
	int sockfd, newsockfd, puerto, clilen, pid;
	struct sockaddr_in serv_addr, cli_addr;
	static int flag_user_ready_log;
	flag_user_ready_log = 0;
	
	/* Cargo mis dos usuarios*/
	cargo_usuarios();
	

	

	if ((sockfd = socket( AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error en creación de  socket");
		exit(1);
	}

	memset( (char *) &serv_addr, 0, sizeof(serv_addr) );
	puerto = atoi( "8282" );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( puerto );
	if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {
		perror( "ligadura" );
		exit( 1 );
	}

	printf("Proceso: %d - socket disponible: %d\n", getpid(), serv_addr.sin_port);

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
					printf("%s",prompt_user_log);
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

