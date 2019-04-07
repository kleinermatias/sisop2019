#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>

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

struct timeval time1, time2;

struct Satelite
{
	char id[TAM];
	char uptime[TAM];
	char version[TAM];
	char consumo[TAM];
};

static struct Satelite sat;
int loggin(int sockfd);
void func_detect(int sockfd);
void send_to_server(int sockfd);
void send_to_server_ack(int sockfd);
int update_firmware(int sockfd);
void status_sat_read(void);
void firm_read(void);
int obtener_telemetria(int sockfd);
int start_scanning(int sockfd);

int main(int argc, char *argv[])
{
	int sockfd, servlen;
	struct sockaddr_un serv_addr;
	int control_error;
	char buffer[TAM];

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

	/** 
	 * Llamo a la funcion log. 
	**/

	

	status_sat_read();

	//fflush(stdout);
	control_error = loggin(sockfd);

	if (control_error == 1)
	{
		bzero(hostname, sizeof(hostname));
		bzero(buffer, sizeof(buffer));
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

	bzero(buffer, sizeof(buffer));
	fgets(buffer, TAM - 1, stdin);
	control_error = write(sockfd, buffer, sizeof(buffer));
	if (control_error < 0)
	{
		printf("Error escribir en socket\n");
		exit(0);
	}
}

void send_to_server_ack(int sockfd)
{
	static int control_error;
	control_error = write(sockfd, "ACK", sizeof("ACK"));
	if (control_error < 0)
	{
		printf("Error escribir en socket\n");
		exit(0);
	}
}

void func_detect(int sockfd)
{
	static char buffer[TAM];
	static int control_error;
	bzero(buffer, sizeof(buffer));
	control_error = read(sockfd, buffer, TAM);
	if (control_error < 0)
	{
		perror("lectura de socket");
		exit(1);
	}
	if (strcmp(buffer, "recibi update") == 0)
	{
		control_error = update_firmware(sockfd);
		if (control_error < 0)
		{
			exit(1);
		}
		else
		{
			printf("Update completo.\n");
		}
	}
	else if (strcmp(buffer, "recibi start scanning") == 0)
	{
		printf("asasasas completo.\n");
		start_scanning(sockfd);
	}

	else if (strcmp(buffer, "recibi obtener telemetria") == 0)
	{
		status_sat_read();
		obtener_telemetria(sockfd);
	}
}

void status_sat_read()
{
	FILE *firmwareBIN;
	FILE *top_file;
	char aux[TAM] = {0};
	char aux_pid[TAM] = {0};

	bzero(sat.id, sizeof(sat.id));
	bzero(sat.uptime, sizeof(sat.uptime));
	bzero(sat.consumo, sizeof(sat.consumo));
	bzero(sat.version, sizeof(sat.version));

	strcpy(sat.id, "ARSAT");
	
	
	gettimeofday(&time2, NULL);
		   
	
	sprintf(aux, "%0.2f", (double)(time2.tv_usec - time1.tv_usec) / 1000000 +
			   (double)(time2.tv_sec - time1.tv_sec));

	strcpy(sat.uptime, aux);

	firmwareBIN = fopen("./clienteBIN/firmware.bin", "rb");
	bzero(aux, sizeof(aux));
	fread(aux, 1, sizeof(aux) - 1, firmwareBIN);
	strcpy(sat.version, aux);
	fclose(firmwareBIN);

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

int update_firmware(int sockfd)
{

	int recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;
	char binaryarray[TAM];
	FILE *binary;
	FILE *firmwareBIN;

	send_to_server_ack(sockfd); //sino hacia 2 write seguidos y se rompia todo

	//Find the size of the binary
	stat = read(sockfd, &size, sizeof(size));

	printf("Packet received.\n");
	printf("Packet size: %i\n", stat);
	printf("binary size: %i\n", size);
	printf(" \n");
	fflush(stdout);

	//Send our verification signal
	write(sockfd, &size, sizeof(size));

	printf("Reply sent\n");
	printf(" \n");

	binary = fopen("./clienteBIN/firmware.bin", "wb");

	if (binary == NULL)
	{
		printf("Error has occurred. binary file could not be opened\n");
		return -1;
	}

	while (recv_size < size)
	{

		bzero(binaryarray, sizeof(binaryarray));
		read_size = read(sockfd, binaryarray, TAM);
		printf("Paquete: %i\n", packet_index);

		//Write the currently read data into our binary file
		write_size = fwrite(binaryarray, 1, read_size, binary);

		if (read_size != write_size)
		{
			printf("error in read write\n");
			return -1;
		}

		//Increment the total number of bytes read
		recv_size += read_size;
		packet_index++;

		printf("Total received binary size: %i\n", recv_size);
		printf(" \n");
	}

	fclose(binary);
	printf("Binario recibido. Reiniciando\n");

	firmwareBIN = fopen("./clienteBIN/firmware.bin", "rb");
	bzero(binaryarray, sizeof(binaryarray));
	fread(binaryarray, 1, sizeof(binaryarray) - 1, firmwareBIN);
	strcpy(sat.version, binaryarray);
	fclose(firmwareBIN);

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

	int n, loop;
	loop = 1;

	int user_or_pass = 0;

	while (loop == 1)
	{
		bzero(buffer, sizeof(buffer));
		bzero(hostname, sizeof(hostname));
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
		bzero(buffer, sizeof(buffer));
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
			loop = 0;
			return 1;
		}

		else
		{
			loop = 1;
		}
	}
	if (terminar)
	{
		printf("Finalizando ejecución\n");
		exit(0);
	}

	return 0;
}


/**
 * @brief Funcion encargada de actualizar el firmware del cliente. Lee el binario y se lo envia.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int obtener_telemetria(int socket)
{
	int control_error;
	char buffer[TAM];

	control_error = write(socket, sat.id, sizeof(sat.id));
	if (control_error < 0)
	{
		perror("escritura de socket");
		exit(1);
	}
	control_error = read(socket, buffer, sizeof(buffer));
	if (control_error < 0)
	{
		perror("lectura de socket");
		exit(1);
	}


	control_error = write(socket, sat.uptime, sizeof(sat.uptime));
	if (control_error < 0)
	{
		perror("escritura de socket");
		exit(1);
	}
	control_error = read(socket, buffer, sizeof(buffer));
	if (control_error < 0)
	{
		perror("lectura de socket");
		exit(1);
	}

	control_error = write(socket, sat.version, sizeof(sat.version));
	if (control_error < 0)
	{
		perror("escritura de socket");
		exit(1);
	}
	control_error = read(socket, buffer, sizeof(buffer));
	if (control_error < 0)
	{
		perror("lectura de socket");
		exit(1);
	}

	control_error = write(socket, sat.consumo, sizeof(sat.consumo));
	if (control_error < 0)
	{
		perror("escritura de socket");
		exit(1);
	}
	control_error = read(socket, buffer, sizeof(buffer));
	if (control_error < 0)
	{
		perror("lectura de socket");
		exit(1);
	}

	printf("%s\n", sat.id);
	printf("%s\n", sat.uptime);
	printf("%s\n", sat.consumo);
	printf("%s\n", sat.version);


	return 1;

}


/**
 * @brief Funcion encargada de actualizar el firmware del cliente. Lee el binario y se lo envia.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int start_scanning(int socket)
{
	FILE *binary;
	int size, read_size, packet_index;
	char send_buffer[2], buffer[TAM];
	packet_index = 1;

	sleep(1);

	binary = fopen("./serverBIN/foto.jpg", "rb");
	printf("Obteniendo tamanio de binario..\n");

	if (binary == NULL)
	{
		printf("Error abriendo binario");
	}

	fseek(binary, 0, SEEK_END);
	size = ftell(binary);
	fseek(binary, 0, SEEK_SET);
	printf("Tamanio total del binario: %i\n", size);

	//Send binary Size
	printf("Enviando tamanio %d\n", size);

	write(socket, &size, sizeof(size));

	//Send binary as Byte Array
	printf("Enviando binario\n");

	read(socket, &buffer, sizeof(buffer));

	while (!feof(binary))
	{

		//Read from the file into our send buffer
		read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, binary);

		//Send data through our socket
		write(socket, send_buffer, read_size);

		printf("Packet Number: %i\n", packet_index);
		printf("Packet Size Sent: %i\n", read_size);
		printf("\n");

		packet_index++;

		//Zero out our send buffer
		bzero(send_buffer, sizeof(send_buffer));
	}
	
	return 1;

}