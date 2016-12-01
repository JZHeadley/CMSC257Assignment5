#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


int flag=0;
int status=0;
int server, client;

void signal_handler(int signNum){
    if (status==0) {
        close(server);
        raise(SIGKILL);
    } else{
        flag=1;
    }
}

int writeFileToClient(int client, char fileName[50]){
    printf("writing file to client\n");
    FILE *fileRequested=NULL;
    int flag=1;
    int i;
    char c;
    char buffer[50];
    setbuf(stdout, NULL);

    printf("opening file\n");
    fileRequested=fopen(fileName,"r");

    while(flag == 1){
        for(i=0;i<50;i++)
            buffer[i]=0;
        for(i=0;i<50;i++){
            if((c=fgetc(fileRequested))==EOF){
                flag = 0;
                break;
            }
            buffer[i] = c;
        }

        if(write( client, buffer, 50) != 50){
            printf("invalid write\n");
            fclose(fileRequested);
            return -1;
        }
    }

    for(i=0;i<50;i++)
        buffer[i]=0;

    write(client,"cmsc257",sizeof("cmsc257"));

    printf("closing file\n");
    fclose(fileRequested);
    return 0;
}

int server_operation( void ) {
    uint32_t inet_len;
    char buffer[50];
    struct sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(2172);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    server = socket(PF_INET, SOCK_STREAM, 0);
    pid_t childPid;
    int childCount=0;
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

    while ( flag==0 ) {
        inet_len = sizeof(caddr);
        if ( (client = accept( server, (struct sockaddr *)&caddr, &inet_len )) == -1 ) {
            printf( "Error on client accept [%s]\n", strerror(errno) );
            close(server);
            return( -1 );
        }
        status=1;
        printf("Creating child\n");
        childCount++;
        childPid = fork();


        if (childPid == 0){
            printf( "Server new client connection [%s/%d]", inet_ntoa(caddr.sin_addr), caddr.sin_port );

            if ( read( client, buffer, 50) != 50 ) {
                printf( "Error writing network data [%s]\n", strerror(errno) );
                close(server);
                return( -1 );
            }

            printf( "\nRequested file is [%s]\n",buffer );

            if( writeFileToClient(client, buffer) !=0){
                printf( "Error writing network data [%s]\n", strerror(errno) );
                close(server);
                return( -1 );
            }
            printf("closing client connection\n");
            close(client); // Close the socket
            exit(0);
        }
        status=0;

        close(client); // Close the socket
    }

    return ( 0 );
}

int main(int argc, char *argv[]){
    FILE *file;
    file = fopen("serverPid","w");
    fprintf(file,"%i",getpid());
    fclose(file);

    signal(SIGINT,signal_handler);

    server_operation();
}
