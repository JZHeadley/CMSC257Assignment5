#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int client_operation( void ) {
    int socket_fd;
    // uint32_t value;
    char *value;
    char buffer[50];
    struct sockaddr_in caddr;
    char *ip = "127.0.0.1";


    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(2172);

    if ( inet_aton(ip, &caddr.sin_addr) == 0 ) {
        return( -1 );
    }

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        printf( "Error on socket creation [%s]\n", strerror(errno) );
        return( -1 );
    }

    if ( connect(socket_fd, (const struct sockaddr *)&caddr, sizeof(caddr)) == -1 ) {
        printf( "Error on socket connect [%s]\n", strerror(errno) );
        return( -1 );
    }

    value = "asdf";
    strncpy(buffer,value,sizeof(buffer));

    if ( write( socket_fd, buffer, sizeof(buffer)) != sizeof(buffer) ) {
        printf( "Error writing network data [%s]\n", strerror(errno) );
        return( -1 );
    }

    printf( "Sent a value of [%s]\n", buffer );
    if ( read( socket_fd, buffer, sizeof(buffer)) != sizeof(buffer) ) {
        printf( "Error reading network data [%s]\n", strerror(errno) );
        return( -1 );
    }

    printf( "Receivd a value of [%s]\n", buffer );
    close(socket_fd); // Close the socket
    return( 0 );

}



int main(int argc, char *argv[]){
    FILE *file;
    file = fopen("clientPid","w");
    fprintf(file,"%i",getpid());
    fclose(file);

    client_operation();
}

