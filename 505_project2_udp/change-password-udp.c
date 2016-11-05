/* UDP client in the internet domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void error(const char *);

int main(int argc, char *argv[])
{
    unsigned int length;
    struct sockaddr_in server, from;
    struct hostent *hp;
    
    int sock, n,n1,n2,n3,n4;
    char buffer1[256],buffer2[256],buffer3[256],buffer4[256];
    
    if (argc != 3) { printf("Usage: server port\n");
        exit(1);
    }
    sock= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("socket");
    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if (hp==0) error("Unknown host");
    bcopy((char *)hp->h_addr,(char *)&server.sin_addr,hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length=sizeof(struct sockaddr_in);
    
    //sending client id
    n=sendto(sock,"1",1,0,(const struct sockaddr *)&server,length);
    
    //sending username
    printf("Please enter the username: ");
    bzero(buffer1,256);
    scanf("%s",buffer1);
    n1=sendto(sock,buffer1,strlen(buffer1),0,(const struct sockaddr *)&server,length);
    if (n1 < 0) error("Not Sending username");
    
    //sending old password
    printf("Please enter the old_password: ");
    bzero(buffer2,256);
    scanf("%s",buffer2);
    n2=sendto(sock,buffer2,strlen(buffer2),0,(const struct sockaddr *)&server,length);
    if (n < 0) error("Not Sending old password");
    
    //sending new password
    printf("Please enter the new_password: ");
    bzero(buffer3,256);
    scanf("%s",buffer3);
    n3=sendto(sock,buffer3,strlen(buffer3),0,(const struct sockaddr *)&server,length);
    if (n < 0) error("Not Sending new password");
    
    // receive TRUE/FALSE/ERROR
    
    n4 = recvfrom(sock,buffer4,256,0,(struct sockaddr *)&from, &length);
    if (n < 0) error("recvfrom");
    
    write(1,"Got an ack: ",12);
    write(1,buffer4,n4);
    
    close(sock);
    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}