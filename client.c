#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>


/*  Global constants  */

#define MAX_LINE           (1000)

ssize_t Readline(int, void *, size_t);
ssize_t Writeline(int, const void *, size_t);

/*  main()  */

int main(int argc, char *argv[]) {

    int       		conn_s;                 /*  connection socket         */
    int 		portno;                 /*  port number               */
    struct    		sockaddr_in servaddr;   /*  socket address structure  */
    char      		buffer[MAX_LINE];       /*  character buffer          */
    char		msg[MAX_LINE + 6];		/*  for socket read and write */
    struct hostent      *server;                /*  Holds remote IP address   */
    char		user_entry[10]; 	/*  for user entered command  */


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

    char c = '0';
    do{
	memset(user_entry, 0, sizeof(user_entry));
	printf("Enter 's' for a string request: \n");
	printf("Enter 't' for a file request: \n");
	printf("Enter 'q' to quit.\n");
        //c = getchar();
	scanf("%s", user_entry);
	//fflush(stdin);
	printf("user_entry:  |%s|", user_entry);
        getchar();
	if((strcmp(user_entry, "s") == 0) || (strcmp(user_entry, "s\n") == 0)){
	    
	    // Get string to echo from user  
    	    printf("Enter the string to echo: ");
	    strncpy(msg, "CAP\n", 5);
            fgets(buffer, MAX_LINE, stdin);
	    strcat(msg, buffer);
	    strcat(msg, "\n");
	    printf(msg);
    	    // Send string to echo server, and retrieve response 
    	    Writeline(conn_s, msg, strlen(msg));
    	    Readline(conn_s, msg, MAX_LINE-1);
            // Output echoed string 
            printf("Echo response: %s\n", msg);
	    fflush(stdin);
	} 
	else if((strcmp(user_entry, "t") == 0)|| (strcmp(user_entry, "t\n") == 0)){
	    printf("Enter the file name: \n");
	} 
	else if (!(strcmp(user_entry, "q") == 0)){
	    printf("Invalid Input!\n");
	}
	
    } while(!(strcmp(user_entry, "q") == 0));

    return EXIT_SUCCESS;
}

/*  Read a line from a socket  */

ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ ) {
	
	if ( (rc = read(sockd, &c, 1)) == 1 ) {
	    *buffer++ = c;
	    if ( c == '\n' )
		break;
	}
	else if ( rc == 0 ) {
	    if ( n == 1 )
		return 0;
	    else
		break;
	}
	else {
	    if ( errno == EINTR )
		continue;
	    return -1;
	}
    }

    *buffer = 0;
    return n;
}


/*  Write a line to a socket  */

ssize_t Writeline(int sockd, const void *vptr, size_t n) {
    size_t      nleft;
    ssize_t     nwritten;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while ( nleft > 0 ) {
	if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
	    if ( errno == EINTR )
		nwritten = 0;
	    else
		return -1;
	}
	nleft  -= nwritten;
	buffer += nwritten;
    }

    return n;
}
