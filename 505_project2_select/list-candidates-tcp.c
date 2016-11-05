#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXBUFLEN 128

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd=0, sizeofbuffer=0,portno,n, n1,n4;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buffer[128];
    
    //preventing read write buffering
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (int[]){128}, sizeof(int));

    if (argc < 3) {
        fprintf(stderr,"usage %s <hostname> <port> \n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    
    //start
    bzero(buffer,128);
    sizeofbuffer = sizeof(buffer);
    
    //n = write(sockfd,"4",1);
    strcpy(buffer,"4");
    strcat(buffer,"\%");
    
    //sending client id
    n = write(sockfd,buffer,sizeof(char)* 128);
    if (n <0 ) error("error writing to socket");
    
    // read from server
    bzero(buffer,MAXBUFLEN);
    n4 = read(sockfd,buffer,MAXBUFLEN);
       if (n4 < 0) error("ERROR reading from socket");
    printf("%s\n",buffer);
    fflush(stdout);
    
    //end
    close(sockfd);
    return 0;
}