#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/*int getFileFromServer(int socket_fd,);*/
int handleServerResponse(int socket_fd, char *buffer, int sizeOfBuffer, char *requestedFileName){
    int i;
    FILE *serverResponseFile;
    serverResponseFile = fopen(requestedFileName,"w"); // might not handle binary and might need to be wb instead

    while(1){

        if ( read( socket_fd, buffer, 50) != 50 ) {
            if(strstr(buffer,"cmsc257")){
                return 1;
            }
            printf( "Error reading network data: %s with [%s]\n", buffer, strerror(errno) );
            return( -1 );
        }
        printf("%s",buffer);
        for(i=0;i<sizeOfBuffer;i++){
            fputc(buffer[i],serverResponseFile);
        }
    }
    // printf( "Receivd a value of [%s]\n", buffer );

    return 0;
}

int client_operation(char requestedFileName[50]) {
    int socket_fd;
    char buffer[50];
    struct sockaddr_in caddr;
    char *ip = "127.0.0.1";

    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(2173);

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

    if ( write( socket_fd, requestedFileName, 50) != 50 ) {
        printf( "Error writing network data [%s]\n", strerror(errno) );
        return( -1 );
    }
    printf( "Requesting a file named: [%s]\n",requestedFileName );

    handleServerResponse(socket_fd,buffer,sizeof(buffer),requestedFileName);

    close(socket_fd); // Close the socket
    return( 0 );

}



int main(int argc, char *argv[]){
    FILE *file;
    file = fopen("clientPid","w");
    fprintf(file,"%i",getpid());
    fclose(file);
    if(argc==1){
        return -1;
    } else{
        client_operation(argv[1]);
    }
    remove("clientPid");
}


