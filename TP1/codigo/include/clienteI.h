#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
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
#define UDP_TAM 1400



struct timeval time1, time2;

struct Satelite
{
	char id[TAM];
	char uptime[TAM];
	char version[TAM];
	char consumo[TAM];
};

static struct Satelite sat;
void func_detect(int sockfd,char *argv[]);
void send_to_server(int sockfd);
void send_to_server_ack(int sockfd);
int update_firmware(int sockfd, char *argv[]);
void status_sat_read(void);
void firm_read(void);
int obtener_telemetria(int socket, int size, struct sockaddr_in prueba);
int start_scanning(int sockfd);





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

void func_detect(int sockfd, char *argv[])
{
	char buffer[TAM] = {0};
	int control_error = 0;
	

	control_error = read(sockfd, buffer, sizeof(buffer));
	if (control_error < 0)
	{
		perror("lectura de socket");
		exit(1);
	}
	else if (strcmp(buffer, "update firmware.bin") == 0)
	{
		control_error = update_firmware(sockfd,argv);
		if (control_error < 0)
		{
			exit(1);
		}
		else
		{
			printf("Update completo.\n");
		}
	}
	else if (strcmp(buffer, "start scanning") == 0)
	{
		
		start_scanning(sockfd);
	}

	else if (strcmp(buffer, "obtener telemetria") == 0)
	{
		status_sat_read();
		int descriptor_socket;
		struct sockaddr_in struct_cliente;
		int tamano_direccion;
		struct hostent *server;
		server = gethostbyname( argv[1] );
        printf("obteniendo telemetrı́a\n");
        /* Creacion de socket */
		if(( descriptor_socket = socket(AF_INET, SOCK_DGRAM, 0) ) < 0 ) 
			perror("socket" );

		/* Inicialización y establecimiento de la estructura del cliente */
		memset( &struct_cliente, 0, sizeof( struct_cliente ) );
		struct_cliente.sin_family = AF_INET;
		struct_cliente.sin_port = htons( 8183  );
		struct_cliente.sin_addr = *( (struct in_addr *)server->h_addr );
		memset( &(struct_cliente.sin_zero), '\0', 8 );

        tamano_direccion = sizeof( struct sockaddr );
		
		obtener_telemetria(descriptor_socket,tamano_direccion,struct_cliente);
		close(descriptor_socket);
	}
	else {
		printf("Error en la detec de func.\n");
		exit(1);
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

int update_firmware(int sockfd,char *argv[])
{

	int recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;
	char binaryarray[TAM];
	FILE *binary;

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

    write(sockfd, "ACK", sizeof("ACK"));
	
	close(sockfd);
	execvp(argv[0],argv);
	
	exit(0);

	return 1;
}



/**
 * @brief Funcion encargada de actualizar el firmware del cliente. Lee el binario y se lo envia.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int obtener_telemetria(int socket, int size, struct sockaddr_in prueba)
{
	int control_error;
	char buffer[TAM] = {0};

	//printf("MANDO\n");
	//sendto( socket, "ACK", sizeof("ACK"), 0, (struct sockaddr *)&prueba, sizeof(prueba) );
	//printf("RECIBO\n");

	strcpy(buffer, "ID: ");
	strcat(buffer, sat.id);
	strcat(buffer, "\n");
	strcat(buffer, "UPTIME: ");
	strcat(buffer, sat.uptime);
	strcat(buffer, "\n");
	strcat(buffer, "VERSION FIRMWARE: ");
	strcat(buffer, sat.version);
	strcat(buffer, "CONSUMO: ");
	strcat(buffer, sat.consumo);
	strcat(buffer, "\n");
	
	printf("%s\n", buffer);
	control_error = sendto( socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&prueba, size );
	
	if (control_error < 0)
	{
		perror("escritura de socket");
		exit(1);
	}
	
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
	char send_buffer[UDP_TAM] = {0};
	char buffer[TAM] = {0};
	packet_index = 1;

	send_to_server_ack(socket); 

	binary = fopen("./clienteBIN/foto.jpg", "rb");
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

		/*
		printf("Packet Number: %i\n", packet_index);
		printf("Packet Size Sent: %i\n", read_size);
		printf("\n");
		*/

		packet_index++;

		//Zero out our send buffer
		bzero(send_buffer, sizeof(send_buffer));
	}
	printf("Se envio la jotooo\n");
	return 1;

}
