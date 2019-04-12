#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <termios.h>
#include <unistd.h>

#define TAM 1024
#define UDP_TAM 64000
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
} usuarios;

static char prompt_user_log[TAM] = {0};
struct Users arregloUsuarios[2];



int autenticacion();
int obtener_funcion(int sockfd);
int update_firmware(int sockfd);
int start_scanning(int sockfd);
int obtener_telemetria(int socket, socklen_t size, struct sockaddr_in prueba);
void cargo_usuarios(void);

/**
 * @brief Funcion encargada de cargar los usuarios. Simula una base de datos. Se llama en el main.
 * @author Kleiner Matias
 * @date 30/03/2019
 */
void cargo_usuarios(void)
{
    struct Users usuario1, usuario2;
    strcpy(usuario1.id, "matias");
    strcpy(usuario1.pass, "ragnar");
    strcpy(usuario2.id, "agus");
    strcpy(usuario2.pass, "anij");
    arregloUsuarios[0] = usuario1;
    arregloUsuarios[1] = usuario2;
}

/**
 * @brief Funcion encargada del logueo en el servidor. Se mantiene en un while hasta los 3 intentos de ingresar user-pass. 
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int autenticacion()
{
    //guardo id y usuario que llegan en estos buffer.
    char autenticacion_id[TAM] = {0};
    char autenticacion_pass[TAM] = {0};
    char *name;
    char hostname[TAM] = {0};
    char prompt[TAM] = {0};
    char buffer[TAM] = {0};

    int comparo_id_usuario, comparo_pass_usuario, contador_intentos;
    struct termios old, new;

    contador_intentos = 1; // inicio en 1. Con cada intento de user-pass incremento. Cuando llego a 4, mando FIN y cierro cliente.

    name = getlogin();
    gethostname(hostname, sizeof(hostname));

    strcpy(prompt, "[");
    strcat(prompt, ANSI_COLOR_GREEN);
    strcat(prompt, name);
    strcat(prompt, "@");
    strcat(prompt, hostname);
    strcat(prompt, ANSI_COLOR_RESET);
    strcat(prompt, "]");
    strcat(prompt, "$ ");
    printf("%s",prompt);
    fflush(stdout);
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n");
    strcpy(autenticacion_id, buffer);

    while (contador_intentos <= 4)
    {

        bzero(buffer, sizeof(buffer));
        printf("[");
        printf(ANSI_COLOR_GREEN);
        printf("%s",name);
        printf("@");
        printf("%s",hostname);
        printf(ANSI_COLOR_RESET);
        printf("]");
        printf("$ ");
        fflush(stdout);

        /* Turn echoing off and fail if we can’t. */
        if (tcgetattr(fileno(stdin), &old) != 0)
        {
            return 0;
        }

        new = old;
        new.c_lflag &= ~ECHO;
        if (tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0)
        {
            return 0;
        }

        /* Read the passphrase */

        fgets(buffer, sizeof(buffer), stdin);
        /* Restore terminal. */
        (void)tcsetattr(fileno(stdin), TCSAFLUSH, &old);

        strtok(buffer, "\n");
        strcpy(autenticacion_pass, buffer);

        //recorro en un for los usuarios. Si encuentro alguno, uso el pass asignado para ese usuario y comparo con lo que recibi.
        for (int i = 0; i < (sizeof(arregloUsuarios) / sizeof(usuarios)); i++)
        {
            comparo_id_usuario = strcmp(arregloUsuarios[i].id, autenticacion_id);
            if (comparo_id_usuario == 0)
            {
                comparo_pass_usuario = strcmp(arregloUsuarios[i].pass, autenticacion_pass);
                if (comparo_pass_usuario == 0)
                {
                    //retorno 1 en caso de coincidencia user-pass.
                    printf("\n");
                    strcpy(prompt_user_log, "[");
                    strcat(prompt_user_log, ANSI_COLOR_GREEN);
                    strcat(prompt_user_log, arregloUsuarios[i].id);
                    strcat(prompt_user_log, "@");
                    strcat(prompt_user_log, hostname);
                    strcat(prompt_user_log, ANSI_COLOR_RESET);
                    strcat(prompt_user_log, "]");
                    strcat(prompt_user_log, "$ ");

                    return 1;
                }
                else
                {
                    printf("ERROR, reintente\n");
                }
            }
        }

        contador_intentos++;

        if (contador_intentos == 4)
        {
            printf("Reintente nuevamente.. \n");
            return 0;
        }
    }
    return 0;
}

/**
 * @brief Funcion que se encarga de parsear el comando del usuario y ejecutar la instruccion.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int obtener_funcion(int sockett)
{
    char buffer[TAM] = {0};
    int socket_server;
    fgets(buffer, sizeof(buffer), stdin);
    strtok(buffer, "\n");
    if (!strcmp("fin", buffer))
    {
        printf("PROCESO %d. Como recibí 'fin', termino la ejecución.\n", getpid());
        exit(0);
    }

    else if (strcmp(buffer, "update firmware.bin") == 0)
    {
        printf("actualizando...\n");
        update_firmware(sockett);
        return (1);
    }

    else if (strcmp(buffer, "start scanning") == 0)
    {

        printf("start scanning\n");
        start_scanning(sockett);
        return (1);
    }

    else if (strcmp(buffer, "obtener telemetria") == 0)
    {   

	struct sockaddr_in struct_servidor;
	socklen_t tamano_direccion;
    write(sockett, "obtener telemetria", sizeof("obtener telemetria"));
	
        printf("obteniendo telemetrı́a\n");
        /* Creacion de socket */
        if ((socket_server = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror("socket");
            exit(1);
        }
        /* Remover el nombre de archivo si existe */
        unlink("8083");
        /* Inicialización y establecimiento de la estructura del servidor */
        memset(&struct_servidor, 0, sizeof(struct_servidor));
        struct_servidor.sin_family = AF_INET;
       struct_servidor.sin_port = htons( 8183 );
        /* Ligadura del socket de servidor a una dirección */
        if( bind( socket_server, (struct sockaddr *) &struct_servidor, sizeof(struct_servidor) ) < 0 ) {
            perror( "bind" );
            exit(1);
        }

        printf( "Socket disponible: %d\n", struct_servidor.sin_port );

        tamano_direccion = sizeof( struct_servidor );
        obtener_telemetria(socket_server,tamano_direccion,struct_servidor);
        close(socket_server);
        return (1);
    }

    else
    {
        printf("comando incorrecto \n");
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
int update_firmware(int socket)
{

    FILE *binary;
    int size, read_size, packet_index;
    char send_buffer[TAM], buffer[TAM];
    packet_index = 1;

    write(socket, "update firmware.bin", sizeof("update firmware.bin"));

    //espero ack
    read(socket, buffer, sizeof(buffer));
    if (strcmp(buffer, "ACK") == 0)
    {
        printf("\nServidor recibio ACK.\n");
    }
    else
    {
        printf("\nError en update_firmware.\n");
    }

    binary = fopen("./serverBIN/firmware.bin", "rb");
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

    bzero(buffer, sizeof(buffer));
    read(socket, buffer, sizeof(buffer));
    if (strcmp(buffer, "ACK") == 0)
    {
        printf("\nServidor recibio ACK.\n");
        close(socket);
    }
    exit(0);
    return 0;
}

/**
 * @brief Funcion encargada de actualizar el firmware del cliente. Lee el binario y se lo envia.
 * @author Kleiner Matias
 * @param socket El descriptor del socket por donde se reciben los datos y se escribe la respuesta a los mismos.
 * @date 30/03/2019
 * @return 1 en caso correcto, @c 0 otherwise.
 */
int obtener_telemetria(int socket, socklen_t size, struct sockaddr_in prueba)
{
    
    char buffer[TAM];
    int control_error;
    
    control_error = recvfrom ( socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&prueba, &size );
    if (control_error < 0)
    {
        perror("lectura de socket");
        exit(1);
    }
    printf("%s",buffer);

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
    int recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;
    char binaryarray[UDP_TAM] = {0};
    char buffer[TAM] = {0};
    FILE *binary;

    sleep(1);

    write(socket, "start scanning", sizeof("start scanning"));

    //espero ack
    read(socket, buffer, sizeof(buffer));
    if (strcmp(buffer, "ACK") == 0)
    {
        printf("\nServidor recibio ACK.\n");
    }
    else
    {
        printf("\nError en update_firmware.\n");
    }

    //Find the size of the binary
    stat = read(socket, &size, sizeof(size));

    printf("Packet received.\n");
    printf("Packet size: %i\n", stat);
    printf("binary size: %i\n", size);
    printf(" \n");
    fflush(stdout);

    //Send our verification signal
    write(socket, &size, sizeof(size));

    printf("Reply sent\n");
    printf(" \n");

    binary = fopen("./servidorBIN/foto.jpg", "wb");

    if (binary == NULL)
    {
        printf("Error has occurred. binary file could not be opened\n");
        return -1;
    }

    while (recv_size < size)
    {

        bzero(binaryarray, sizeof(binaryarray));
        read_size = read(socket, binaryarray, sizeof(binaryarray));
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
	printf("Total received binary size: %i\n", read_size);
       // printf("Total received binary size: %i\n", recv_size);
        printf(" \n");
    }

    fclose(binary);
    printf("Binario recibido. Reiniciando\n");

    return 1;
}
