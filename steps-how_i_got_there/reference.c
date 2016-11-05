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

int sockfd;
struct sockaddr_in my_addr;
struct sockaddr_in your_add;
socklen_t addr_len;

int numbytes1, numbytes2,x ;
int count = 0;

char buf1[MAXBUFLEN], buf2[MAXBUFLEN];

struct node{
    int data,data1,data2,d;
    char *name;
    struct node *next;
    struct node *next1;
};

struct node *head=NULL;
struct node *end=NULL;

struct node *head1=NULL;
struct node *end1=NULL;

void addvoter(int voterid)
{
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    temp->data=voterid;
    temp->d=x;
    temp->next=NULL;
    
    if(head==NULL){
        head=temp;
        end=temp;
        printf("%d\n",temp->data);
        return;
    }else{
        end->next=temp;
        end=temp;
        printf("%d\n",temp->data);
    }
}

void add_candidate(char *name ,int voterid) // add parameter for cand_name
{
    // Check if voter is in authorizing list above list, then
    // check if voter has voted or not
    
    struct node *temp1 = (struct node*)malloc(sizeof(struct node));
    temp1->data1=name;
    temp1->data2=count;
    temp1->next=NULL;
    
    struct node *temp2= head1;
    
    if(head1==NULL){
        head1=temp1;
        end1=temp1;
        printf("%d\n",temp1->data1,"%d\n",temp1->data2);
        return;
    }
    else{
        while (temp2 != NULL) {
            if (temp2->data == voterid) {
                return;
            }
            temp2 = temp2->next;
        }
        
        end1->next=temp;
        end=temp;
        printf("%d\n",temp->data);
    }
}

/*
 void add_candidate(char *name ,int voterid) // add parameter for cand_name
 { // if voter id is in list
 
 
 
 // then add candidate, count ++
 struct node *temp = (struct node*)malloc(sizeof(struct node));
 temp->data=voterid;
 temp->next=NULL;
 
 //temp->data2= candidate name
 
 struct node *temp1= head;
 
 if(head==NULL){
 head=temp;
 end=temp;
 printf("%d\n",temp->data);
 return;
 }
 else{
 while (temp1 != NULL) {
 if (temp1->data == voterid) {
 return;
 }
 temp1 = temp1->next;
 
 // else call the 3 field linked list
 }
 
 end->next=temp;
 end=temp;
 printf("%d\n",temp->data);
 }
 }
 
 void vote_candidate(char *name, int voterid){
 struct node *temp = (struct node*)malloc(sizeof(struct node));
 temp->data1=voterid;
 temp->data2=name;
 temp->next=NULL;
 
 struct node *tempp= head;
 
 if(head==NULL){
 head=temp;
 end=temp;
 printf("%d\n",temp->data);
 return;
 }
 else{
 while (tempp != NULL) {
 if (tempp->data2 == name) {
 int count = count + 1;
 }
 
 }
 
 }
 */
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
        
        if((numbytes2 = recvfrom(sockfd, buf2, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1)            {
            perror("Server-recvfrom() error");
            exit(1);
        }
        else{
            int id;
            id = atoi(buf2);
            addvoter(id);
        }
        
    }
    
    //end1
    if(close(sockfd) == 0)
        printf("Server-sockfd successfully closed!\n");
    return 0;
}