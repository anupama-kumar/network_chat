#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 2866
#define MAXBUFLEN 256

#define TRUE1 1
#define FALSE1 0
#define OK 2
#define EXISTS 3
#define ERROR 4
#define NEW 5
#define NOTAVOTER 6
#define ALREADYVOTED 7
#define UNAUTHORIZED 8
#define VOTE 9


int sockfd;
struct sockaddr_in my_addr;
struct sockaddr_in your_add;
socklen_t addr_len;

char buf1[MAXBUFLEN], buf2[MAXBUFLEN], buf3[MAXBUFLEN], buf_addvoter[MAXBUFLEN], buf_bytes[MAXBUFLEN];
char buf1_votefor[MAXBUFLEN], buf2_votefor[MAXBUFLEN];
int bytes,numbytes3;
int cand, voterid;


struct addvoter{
    int data_voterid,data_bool;
    struct addvoter *next_addvoter;
};

struct votefor {
    char *data_name;
    int data_count;
    struct votefor *next_votefor;
};

// addvoter pointers
struct addvoter *head1 = NULL;
struct votefor *head2 = NULL;

int addvoterid(int voterid){
    
    struct addvoter *temp_addvoter = (struct addvoter*)malloc(sizeof(struct addvoter));
    temp_addvoter->data_voterid= voterid;
    temp_addvoter->data_bool=0;
    temp_addvoter->next_addvoter= NULL;
    
    struct addvoter *iter1= head1;
    
    if(head1== NULL){
        head1 = temp_addvoter;
        printf("%d\n",temp_addvoter->data_voterid);
        return OK;
    }
    // logic to search in existing linked list
    
    while (iter1 != NULL) {
        if (iter1->data_voterid == voterid) {
            return EXISTS;
        }
        iter1 = iter1->next_addvoter;
    }
    
    // logic to add new node with voter id
    
    iter1 = head1;
    while (iter1->next_addvoter !=NULL ) {
        iter1 = iter1->next_addvoter;
    }
    iter1->next_addvoter = temp_addvoter;
    
    printf("%d\n",temp_addvoter->data_voterid);
    return OK;
    
}

void addvoter(){
    
    if((numbytes3 = recvfrom(sockfd, buf_addvoter, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1) {
        perror("Server-recvfrom() error lol!");
        exit(1);
    }
    else {
        int id;
        id = atoi(buf_addvoter);
        addvoterid(id);
    }
    
    bzero(buf_addvoter,255);
}

int create(char *name){
    
    int count;
    
    struct votefor *newCand = (struct votefor*)malloc(sizeof(struct votefor));
    char *data_name = (char *)malloc(sizeof(char));
    
    strcpy(newCand->data_name,name);
    newCand->data_count= 0; // doubt1 or count?
    newCand->next_votefor=NULL;
    
    struct votefor *iter_x = head2;
    if (head2== NULL) {
        head2 = newCand;
        return NEW;
    }
    else if(head2 != NULL){
        while (iter_x !=NULL) {
            if (strcmp(newCand->data_name,name) == 0) {
                // if cand name is in list
                
                count = count +1;
                return EXISTS;
            }
            iter_x = iter_x->next_votefor;
            
            else {
                // if cand name is not there in the list
                // add new node created and increment count
                
                head2->next_votefor = iter_x;
                count = count + 1;
                return NEW;
                
            }
        }
    }
}
// if isCast = 1 --> alreadyvoted
int votefor() {
    
    if((cand= recvfrom(sockfd, buf1_votefor, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1)
    {
        perror("Server-recvfrom() error");
        exit(1);
    }
    
    if((voterid = recvfrom(sockfd, buf2_votefor, MAXBUFLEN-1, 0, ((struct sockaddr *)&your_add), &addr_len)) == -1)
    {
        perror("Server-recvfrom() error");
        exit(1);
    }
    
    int id;
    id = atoi(buf2_votefor);
    
    
    struct addvoter *iter = head1;
while (iter != NULL)
    {
        
    if (iter->data_voterid == id)
        {
    
            // if he has NOT cast vote
            if (iter->data_bool == 0) {
                
                int result = create(buf1_votefor);
                if (result == EXISTS) {
                    iter->data_bool = 1; // how are we sure iter can be modified here?
                    return EXISTS;
                }
                
                if (result == NEW){
                    iter->data_bool = 1;
                    return NEW;
                }
                
            }
            // if he has casted vote
            else {
                return ALREADYVOTED;
            }
            
            
        }
        iter = iter->next_addvoter;
       
    else {
            return NOTAVOTER;
         }
    }
    
    bzero(buf1_votefor,255);
    bzero(buf2_votefor,255);
}


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
    
    
    while (1) {
        if( (bytes = recvfrom(sockfd, buf_bytes, MAXBUFLEN-1, 0,((struct sockaddr *)&your_add), &addr_len ) )== -1 )
        {
            perror("Client ID receive error");
            exit(1);
            
        }
        else
        {
            int x;
            x = atoi(buf_bytes);
            
            switch (x) {
                    
                case 2:
                    //printf("%d\n",x);
                    addvoter();
                    break;
                    
//                case 3:
//                    //printf("%d\n",x);
//                    votefor();
//                    break;
                default:
                    break;
            }
        }
        
        
    }

    bzero(buf_bytes,255);
    
    if(close(sockfd) == 0)
        printf("Server-sockfd successfully closed!\n");
    return 0;
    
}




