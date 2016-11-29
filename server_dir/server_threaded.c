#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>

int server_operation( void ) {
    int server, client;
    uint32_t value, inet_len;
    struct sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(16453);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server = socket(PF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        printf( "Error on socket creation [%s]\n", strerror(errno) );
        return( -1 );
    }
    if ( bind(server, (struct sockaddr *)&saddr, sizeof(saddr)) == -1 ) {
        printf( "Error on socket bind [%s]\n", strerror(errno) );
        return( -1 );
    }
    if ( listen( server, 5 ) == -1 ) {
        printf( "Error on socket listen [%s]\n", strerror(errno) );
        return( -1 );
    }

    while ( 1 ) {
        inet_len = sizeof(caddr);
        if ( (client = accept( server, (struct sockaddr *)&caddr, &inet_len )) == -1 ) {
            printf( "Error on client accept [%s]\n", strerror(errno) );
            close(server);
            return( -1 );
        }
        printf( "Server new client connection [%s/%d]", inet_ntoa(caddr.sin_addr), caddr.sin_port );
        if ( read( client, &value, sizeof(value)) != sizeof(value) ) {
            printf( "Error writing network data [%s]\n", strerror(errno) );
            close(server);
            return( -1 );
        }
        value = ntohl(value);
        printf( "Receivd a value of [%d]\n", value );
        value++;
        value = htonl(value);
        if ( write( client, &value, sizeof(value)) != sizeof(value) ) {
            printf( "Error writing network data [%s]\n", strerror(errno) );
            close(server);
            return( -1 );
        }
        printf( "Sent a value of [%d]\n", value );
        close(client); // Close the socket
    }
    return ( 0 );
}
int main(int argc, char *argv[]){
    server_operation();
}
