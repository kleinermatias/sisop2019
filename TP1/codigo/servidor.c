#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#define TAM 1024

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

struct Users
{
	char id[20];
	char pass[20];
};

struct Users arregloUsuarios[2];

int autenticacion(int socket);
int parse_func(int socket);
int update_firmware(int sockfd);

int main(int argc, char *argv[])
{

	int sockfd, newsockfd, servlen, clilen, n, pid;
	int control_error;
	struct sockaddr_un cli_addr, serv_addr;
	char buffer[TAM];

	struct Users usuario1, usuario2;

	strcpy(usuario1.id, "matias");
	strcpy(usuario1.pass, "ragnar");
	strcpy(usuario2.id, "agus");
	strcpy(usuario2.pass, "anij");
	arregloUsuarios[0] = usuario1;
	arregloUsuarios[1] = usuario2;

	char temp[1024];

	char hostname[1024];

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

			control_error = autenticacion(newsockfd);

			if (control_error == 1)
			{
				printf("Envio user@host \n");
				printf(temp);
				printf("\n");
				n = write(newsockfd, "SI", sizeof("SI"));
				if (n < 0)
				{
					perror("escritura en socket");
					exit(1);
				}
				n = read(newsockfd, buffer, TAM);
				if (n < 0)
				{
					perror("lectura de socket");
					exit(1);
				}

				strcat(temp, "@");
				memset(hostname, 0, sizeof(hostname));
				gethostname(hostname, sizeof(hostname));
				strcat(temp, hostname);

				n = write(newsockfd, temp, sizeof(temp));
				if (n < 0)
				{
					perror("escritura en socket");
					exit(1);
				}
			}
			else if (control_error == 0)
			{
				printf("ERROR en autenticacion.");
				exit(0);
			}

			while (1)
			{
				parse_func(newsockfd);
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

/**
 * @brief Funcion encargada del logueo en el servidor. Se mantiene en un while hasta los 3 intentos de ingresar user-pass. Pasado dichos intentos, envia un FIN al cliente para indicar que se cierre.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int autenticacion(int socket)
{
	//guardo id y usuario que llegan en estos buffer.
	char autenticacion_id[TAM];
	char autenticacion_pass[TAM];

	char buffer[TAM];
	int control_errores, comparo_id_usuario, comparo_pass_usuario, contador_intentos;

	contador_intentos = 1; // inicio en 1. Con cada intento de user-pass incremento. Cuando llego a 4, mando FIN y cierro cliente.

	while (contador_intentos <= 4)
	{
		memset(buffer, '\0', TAM);
		control_errores = read(socket, buffer, TAM);
		if (control_errores < 0)
		{
			perror("lectura de socket");
			exit(1);
		}

		strtok(buffer, "\n");
		strcpy(autenticacion_id, buffer);

		control_errores = write(socket, "Obtuve su mensaje", sizeof("Obtuve su mensaje"));
		if (control_errores < 0)
		{
			perror("escritura en socket");
			exit(1);
		}

		memset(buffer, '\0', TAM);
		control_errores = read(socket, buffer, TAM);
		if (control_errores < 0)
		{
			perror("lectura de socket");
			exit(1);
		}
		strtok(buffer, "\n");
		strcpy(autenticacion_pass, buffer);

		//recorro en un for los usuarios. Si encuentro alguno, uso el pass asignado para ese usuario y comparo con lo que recibi.
		for (int i = 0; i < sizeof(arregloUsuarios); i++)
		{
			comparo_id_usuario = strcmp(arregloUsuarios[i].id, autenticacion_id);
			if (comparo_id_usuario == 0)
			{
				comparo_pass_usuario = strcmp(arregloUsuarios[i].pass, autenticacion_pass);
				if (comparo_pass_usuario == 0)
				{
					//retorno 1 en caso de coincidencia user-pass.
					return 1;
				}
			}
		}

		contador_intentos++;

		if (contador_intentos == 4)
		{
			control_errores = write(socket, "FIN", sizeof("FIN"));
			if (control_errores < 0)
			{
				perror("escritura en socket");
				exit(0);
			}
		}

		else
		{
			control_errores = write(socket, "Obtuve su mensaje", sizeof("Obtuve su mensaje"));
			if (control_errores < 0)
			{
				perror("escritura en socket");
				exit(1);
			}
		}
	}
	return 0;
}

/**
 * @brief Funcion encargada del logueo en el servidor. Se mantiene en un while hasta los 3 intentos de ingresar user-pass. Pasado dichos intentos, envia un FIN al cliente para indicar que se cierre.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int parse_func(int socket)
{
	//guardo id y usuario que llegan en estos buffer.

	char buffer[TAM];
	int control_errores;

	memset(buffer, 0, TAM);
	control_errores = read(socket, buffer, TAM);
	if (control_errores < 0)
	{
		perror("lectura de socket");
		exit(1);
	}
	printf("Recibí: %s \n", buffer);

	strtok(buffer, "\n");
	if (!strcmp("fin", buffer))
	{
		printf("PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid());
		exit(0);
	}

	if (strcmp(buffer, "update firmware.bin") == 0)
	{
		printf("update firmware.bin");
		memset(buffer, '\0', TAM);
		control_errores = write(socket, "recibi update", sizeof("recibi update"));
		if (control_errores < 0)
		{
			perror("escritura en socket");
			exit(1);
		}
		update_firmware(socket);
	}
	else if (strcmp(buffer, "start scanning") == 0)
	{
		printf("start scanning");
		memset(buffer, '\0', TAM);
		control_errores = write(socket, "recibi start scanning", sizeof("recibi start scanning"));
		if (control_errores < 0)
		{
			perror("escritura en socket");
			exit(1);
		}
	}
	else if (strcmp(buffer, "obtener telemetrı́a") == 0)
	{
		printf("obtener telemetrı́a");
		memset(buffer, '\0', TAM);
		control_errores = write(socket, "recibi obtener telemetria", sizeof("recibi obtener telemetria"));
		if (control_errores < 0)
		{
			perror("escritura en socket");
			exit(1);
		}
	}
	else
	{
		printf("incorrecto");
		memset(buffer, '\0', TAM);
		control_errores = write(socket, "incorrecto", sizeof("incorrecto"));
		if (control_errores < 0)
		{
			perror("escritura en socket");
			exit(1);
		}
	}

	fflush(stdout);

	return 0;
}

/**
 * @brief Funcion encargada del logueo en el servidor. Se mantiene en un while hasta los 3 intentos de ingresar user-pass. Pasado dichos intentos, envia un FIN al cliente para indicar que se cierre.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int update_firmware(int socket)
{

	FILE *picture;
	int size, read_size, stat, packet_index;
	char send_buffer[TAM], read_buffer[TAM];
	packet_index = 1;

	picture = fopen("file.txt", "r");
	printf("Getting Picture Size\n");

	if (picture == NULL)
	{
		printf("Error Opening Image File");
	}

	fseek(picture, 0, SEEK_END);
	size = ftell(picture);
	fseek(picture, 0, SEEK_SET);
	printf("Total Picture size: %i\n", size);
	
	
	//Send Picture Size
	printf("Sending Picture Size\n");
	usleep(1000);
	write(socket, &size, sizeof(size));
	
	//Send Picture as Byte Array
	printf("Sending Picture as Byte Array\n");

	do
	{ //Read while we get errors that are due to signals.
		stat = read(socket, &read_buffer, 255);
		printf("Bytes read: %i\n", stat);
	} while (stat < 0);

	printf("Received data in socket\n");

	while (!feof(picture))
	{
		//while(packet_index = 1){
		//Read from the file into our send buffer
		read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, picture);

		//Send data through our socket
		do
		{
			stat = write(socket, send_buffer, read_size);
		} while (stat < 0);

		printf("Packet Number: %i\n", packet_index);
		printf("Packet Size Sent: %i\n", read_size);
		printf(" \n");
		printf(" \n");

		packet_index++;

		//Zero out our send buffer
		bzero(send_buffer, sizeof(send_buffer));
	}

	return 0;
}