#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  Our own helper functions  */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>


/*  Global constants  */

#define MAX_LINE           (1000)


/*  main()  */

int main(int argc, char *argv[]) {

    int       		conn_s;                 /*  connection socket         */
    int 		portno;                 /*  port number               */
    struct    		sockaddr_in servaddr;   /*  socket address structure  */
    char      		buffer[MAX_LINE];       /*  character buffer          */
    struct hostent      *server;                /*  Holds remote IP address   */
    char 		user_entry 		/*  for user entered command  */

    /*  Get command line arguments  */

    /*  eg. ./client 127.0.0.1 7000  */

    //check if command line arguments are valid
    if(argc < 3){
        printf("Invalid Arguments!");
	exit(1);
    }

    //get the remort port number and server address from command line arguments
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);

     if (server == NULL) {
             fprintf(stderr,"ERROR, no such host\n");
             exit(1);
     }
	

    /*  Create the listening socket  */

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(portno);

    /*  Byte copy the address retrieved from server  into the
        server addr structure      */
     bcopy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);

    
    /*  connect() to the remote echo server  */

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
	printf("ECHOCLNT: Error calling connect()\n");
	exit(EXIT_FAILURE);
    }

    //do{} while(user_entry != 'q');


    /*  Get string to echo from user  */

    printf("Enter the string to echo: ");
    fgets(buffer, MAX_LINE, stdin);
    

    /*  Send string to echo server, and retrieve response  */

    Writeline(conn_s, buffer, strlen(buffer));
    Readline(conn_s, buffer, MAX_LINE-1);


    /*  Output echoed string  */

    printf("Echo response: %s\n", buffer);

    return EXIT_SUCCESS;
}
