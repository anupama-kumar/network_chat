/* UDP client  */
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
    
    int sock,n1,n2,n3,n4;
    char buffer4[256];
    
    if (argc != 5) { printf("Usage: <error> <hostname> <portno> <candidate_name> <voterid> port\n");
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
    n1=sendto(sock,"3",1,0,(const struct sockaddr *)&server,length);
    
    //sending candidate name
    n2=sendto(sock,argv[3],strlen(argv[3]),0,(const struct sockaddr *)&server,length);
    if (n2 < 0) error("Sendto");
    
    
    //sending voterid
    n3=sendto(sock,argv[4],strlen(argv[4]),0,(const struct sockaddr *)&server,length);
    if (n3 < 0) error("Sendto");
    
    // receive EXISTS/NOTAVOTER/NEW/ALREADYVOTED/ERROR
    n4 = recvfrom(sock,buffer4,256,0,(struct sockaddr *)&from, &length);
    if (n4 < 0) error("recvfrom");
    
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