
#include "include/clienteI.h"



int main(int argc, char *argv[])
{
	int sockfd, puerto;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	printf("%i\n",getpid());
	/* Inicio referencia de mi var time1, para luego poder hacer la resta de time2-time1 y sacar asi el uptime.*/
	gettimeofday(&time1, NULL);

	if (argc < 2)
	{
		fprintf(stderr, "Falta ip che gorreaaa %s\n", argv[0]);
		exit(0);
	}

	puerto = atoi( "8282" );
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
	}

	server = gethostbyname( argv[1] );
	if (server == NULL) {
		fprintf( stderr,"Error, no existe el host\n" );
		exit( 0 );
	}
	memset( (char *) &serv_addr, '0', sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );
	serv_addr.sin_port = htons( puerto );
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		printf( "%i", (int)serv_addr.sin_port );
		exit( 1 );
	}


	/* Inicio estructura satelite.*/
	status_sat_read();

	while (1)
	{
		/* Siempre me quedo en este while para atender las peticiones del servidor.*/
		func_detect(sockfd,argv);
	}
	return 0;
}


