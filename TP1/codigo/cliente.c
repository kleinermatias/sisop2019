#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define TAM 1024

static char hostname[TAM];
static char user[TAM];

int loggin(int sockfd);
void func_detect(int sockfd);
void send_to_server(int sockfd);
int update_firmware(int sockfd);

int main(int argc, char *argv[])
{
	int sockfd, servlen;
	struct sockaddr_un serv_addr;
	int control_error;
	char buffer[TAM];

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

	/** 
	 * Llamo a la funcion log. 
	**/
	control_error = loggin(sockfd);

	if (control_error == 1)
	{

		memset(hostname, '\0', sizeof(hostname));
		memset(buffer, '\0', sizeof(buffer));
		control_error = read(sockfd, buffer, sizeof(buffer));
		if (control_error < 0)
		{
			perror("lectura de socket");
			exit(1);
		}
		strcpy(hostname, "[");
		strcat(hostname, ANSI_COLOR_YELLOW);
		strcat(hostname, user);
		strcat(hostname, buffer);
		strcat(hostname, ANSI_COLOR_RESET);
		strcat(hostname, "] ");
	}

	else
	{
		printf("Error durante el log, cierro el cliente.\n");
		exit(0);
	}

	while (1)
	{

		printf(hostname);
		fflush(stdout);
		send_to_server(sockfd);
		func_detect(sockfd);
	}
	return 0;
}

void send_to_server(int sockfd)
{
	static char buffer[TAM];
	static int control_error;
	memset(buffer, '\0', TAM);
	fgets(buffer, TAM - 1, stdin);
	control_error = write(sockfd, buffer, sizeof(buffer));
	if (control_error < 0)
	{
		printf("Error escribir en socket\n");
		exit(0);
	}
}

void func_detect(int sockfd)
{
	static char buffr[TAM];
	static int control_error;
	memset(buffr, '\0', TAM);
	control_error = read(sockfd, buffr, TAM);
	if (control_error < 0)
	{
		perror("lectura de socket");
		exit(1);
	}
	if (strcmp(buffr, "recibi update") == 0)
	{
		update_firmware(sockfd);
		fflush(stdout);
	}
	else if (strcmp(buffr, "recibi start scanning") == 0)
	{
	}

	else if (strcmp(buffr, "recibi obtener telemetria") == 0)
	{
	}
}

int update_firmware(int sockfd)
{
	// Start function

	int recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;

	char imagearray[10241];
	FILE *image;

	printf("NADAAAA");
	fflush(stdout);
	
	
	//Find the size of the image
	
	stat = read(sockfd, &size, sizeof(size));
	

	printf("Packet received.\n");
	printf("Packet size: %i\n", stat);
	printf("Image size: %i\n", size);
	printf(" \n");
	fflush(stdout);

	

	//Send our verification signal
	do
	{
		stat = write(sockfd, &size, sizeof(size));
	} while (stat < 0);

	printf("Reply sent\n");
	printf(" \n");

	image = fopen("file1.txt", "w");

	if (image == NULL)
	{
		printf("Error has occurred. Image file could not be opened\n");
		return -1;
	}

	//Loop while we have not received the entire file yet

	struct timeval timeout = {10, 0};

	fd_set fds;
	int buffer_fd;

	while (recv_size < size)
	{
		//while(packet_index < 2){

		FD_ZERO(&fds);
		FD_SET(sockfd, &fds);

		buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

		if (buffer_fd < 0)
			printf("error: bad file descriptor set.\n");

		if (buffer_fd == 0)
			printf("error: buffer read timeout expired.\n");

		if (buffer_fd > 0)
		{
			do
			{
				read_size = read(sockfd, imagearray, 10241);
			} while (read_size < 0);

			printf("Packet number received: %i\n", packet_index);
			printf("Packet size: %i\n", read_size);

			//Write the currently read data into our image file
			write_size = fwrite(imagearray, 1, read_size, image);
			printf("Written image size: %i\n", write_size);

			if (read_size != write_size)
			{
				printf("error in read write\n");
			}

			//Increment the total number of bytes read
			recv_size += read_size;
			packet_index++;
			printf("Total received image size: %i\n", recv_size);
			printf(" \n");
			printf(" \n");
		}
	}

	fclose(image);
	printf("Image successfully Received!\n");
	return 1;
}

/**
 * @brief Funcion encargada del logueo en el cliente. Se mantiene en un while hasta los 3 intentos de ingresar user-pass. Pasado dichos intentos, envia un FIN al cliente para indicar que se cierre.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int loggin(int sockfd)
{
	char buffer[TAM];
	int terminar = 0;

	int n;

	int user_or_pass = 0;

inicio:
	memset(buffer, '\0', TAM);
	memset(hostname, '\0', sizeof(hostname));
	char *name;
	name = getlogin();

	gethostname(hostname, sizeof(hostname));

	printf("[");
	printf(ANSI_COLOR_GREEN);
	printf(name);
	printf("@");
	printf(hostname);
	printf(ANSI_COLOR_RESET);
	printf("]");
	printf("$ ");
	fflush(stdout);

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
	if (user_or_pass == 0)
	{
		strcpy(user, buffer);
		user_or_pass++;
	}
	else
	{
		user_or_pass = 0;
	}

	memset(buffer, '\0', TAM);
	n = read(sockfd, buffer, TAM);
	if (n < 0)
	{
		perror("lectura de socket");
		exit(1);
	}

	//printf("Respuesta: %s\n", buffer);
	if (strcmp(buffer, "FIN") == 0)
	{
		terminar = 1;
	}

	else if (strcmp(buffer, "SI") == 0)
	{
		n = write(sockfd, buffer, sizeof(buffer));
		if (n < 0)
		{
			perror("escritura de socket");
			exit(1);
		}
		return 1;
	}

	else
	{
		goto inicio;
	}

	if (terminar)
	{
		printf("Finalizando ejecución\n");
		exit(0);
	}

	return 0;
}
