#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 4950
#define MAXBUFLEN 500

char user[MAXBUFLEN] = "cis505";
char pass[MAXBUFLEN] = "project2";
int a,b;

int sockfd;
struct sockaddr_in my_addr;
struct sockaddr_in your_add;

socklen_t addr_len;

int numbytes1, numbytes2, numbytes3, voterid;
char buf1[MAXBUFLEN], buf2[MAXBUFLEN], buf3[MAXBUFLEN], buf4[MAXBUFLEN];



//// start1
void password_change(){

    if((numbytes3 = recvfrom(sockfd, buf3, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1) {
            perror("Server-recvfrom() error lol!");
            exit(1);
        }
        else{
            strcpy(pass,buf3);
            printf("New password is : \"%s\"\n",pass);
            
        }
    }
//// end1


// start2

struct node{
    int voterid;
    struct node *next;
};

struct node *head=NULL;
struct node *end=NULL;

void addvoter(int voterid)
{
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    temp->voterid=voterid;
    temp->next=NULL;
    
    if(head==NULL){
        head=temp;
        end=temp;
        printf("%d\n",temp->voterid);
        return;
    }else{
        end->next=temp;
        end=temp;
        printf("%d\n",temp->voterid);
    }
}

//end2

int main(int argc, char *argv[]) {
    
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Server-socket() sockfd error");
        exit(1);
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1){
        perror("Server-bind() error");
        exit(1);
    }
    addr_len = sizeof(struct sockaddr);
    
    //start 1
   while (1) {
        
    if((numbytes1 = recvfrom(sockfd, buf1, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1)            {
                perror("Server-recvfrom() error");
                exit(1);
            }
            buf1[numbytes1] = '\0';
            printf("Username entered is \"%s\"\n", buf1);
        
            if((numbytes2 = recvfrom(sockfd, buf2, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1) {
                perror("Server-recvfrom() error");
                exit(1);
            }
            buf2[numbytes2] = '\0';
            printf("Passowrd entered is  \"%s\"\n", buf2);
        
            a = strcmp(buf1,user);
            b = strcmp(buf2,pass);
            
            if (a == 0 &&  b == 0) {
                password_change();
            }
            else{
                printf("old password was incorrect \n");
            }
    
    //end 1
    
    //start2
    
        if((voterid = recvfrom(sockfd, buf4, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1) {
            perror("Server-recvfrom() error");
            exit(1);
        }
        else {
            int id;
            id = atoi(buf4);
            addvoter(id);
        }

    //end2
   }
    
    if(close(sockfd) == 0)
        printf("Server-sockfd successfully closed!\n");
    return 0;
}