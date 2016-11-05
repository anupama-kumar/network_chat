/* Create a UDP server */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#define MAXBUFLEN 512

#define WRONG 1
#define RIGHT 2
#define EXISTS 3
#define ERROR 4
#define NEW 5
#define NOTAVOTER 6
#define ALREADYVOTED 7
#define NEWCAND 8

int sock, length, n, forhead = 0;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;

char buf1[MAXBUFLEN], buf2[MAXBUFLEN], buf3[MAXBUFLEN], buf_addvoter[MAXBUFLEN];
char buf_bytes[MAXBUFLEN];
char buf1_votefor[MAXBUFLEN],buf2_votefor[MAXBUFLEN],buf_votecount[MAXBUFLEN];
char buf_vresult1[MAXBUFLEN],buf_vresult2[MAXBUFLEN];
char listcand_buf[MAXBUFLEN],votecount_buf[MAXBUFLEN],result_buf[MAXBUFLEN];
int bytes,numbytes1,numbytes2,numbytes3,bytes_vresult1,bytes_vresult2;
int bytes_addvoter,bytes_votecount;
int a,b;
int c,d;

int cand,vid;

char final_buf[512];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

char user[256] = "cis505";
char pass[256] = "project2";


struct addvoter{
    int data_voterid,data_bool;
    struct addvoter *next_addvoter;
};
struct addvoter *head1 = NULL;
struct addvoter *save;

struct votefor {
    char *data_name;
    int data_count;
    struct votefor *next_votefor;
};
struct votefor *head2 = NULL;
struct votefor *nxtptr = NULL;

// CHANGE_PASSWORD

int change_password(){
    
    if((numbytes1 = recvfrom(sock, buf1, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1){
        perror("Server-recvfrom() error");
        exit(1);
    }
    buf1[numbytes1] = '\0';
    
    if((numbytes2 = recvfrom(sock, buf2, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1){
        perror("Server-recvfrom() error");
        exit(1);
    }
    buf2[numbytes2] = '\0';
    
    a = strcmp(user,buf1);
    b = strcmp(pass,buf2);
    
    if (a == 0 &&  b == 0) {
        
        if((numbytes3 = recvfrom(sock, buf3, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1) {
            perror("Server-recvfrom() error ");
            exit(1);
        }
        else {
            strcpy(pass,buf3);
                    }
        sendto(sock,"TRUE",4,0,(const struct sockaddr *)&from,fromlen);
        return RIGHT;
    }
    else {
        sendto(sock,"FALSE",5,0,(const struct sockaddr *)&from,fromlen);
        return WRONG;
    }
    sendto(sock,"ERROR",5,0,(const struct sockaddr *)&from,fromlen);
    return ERROR;
}

// ADDVOTER
void addvoterid(int voterid){
    
    struct addvoter *temp_addvoter = (struct addvoter*)malloc(sizeof(struct addvoter));
    temp_addvoter->data_voterid= voterid;
    temp_addvoter->data_bool=0;
    temp_addvoter->next_addvoter= NULL;
    
    struct addvoter *iter1= head1;
    
    if(head1== NULL){
        head1 = temp_addvoter;
        sendto(sock,"OK",2,0,(const struct sockaddr *)&from,fromlen);
        return;
    }
    while (iter1 != NULL) {
        if (iter1->data_voterid == voterid) {
            sendto(sock,"EXISTS",6,0,(const struct sockaddr *)&from,fromlen);
            return;
        }
        iter1 = iter1->next_addvoter;
    }
    
    iter1 = head1;
    while (iter1->next_addvoter !=NULL ) {
        iter1 = iter1->next_addvoter;
    }
    iter1->next_addvoter = temp_addvoter;
    
    sendto(sock,"OK",2,0,(const struct sockaddr *)&from,fromlen);
    return;
    
}

void addvoter(){
    
    if((bytes_addvoter = recvfrom(sock, buf_addvoter, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1) {
        perror("Server not receiving anything error");
        exit(1);
    }
    else {
        int id;
        id = atoi(buf_addvoter);
        addvoterid(id);
    }
}


// VOTEFOR

int create(char *name){
    
    struct votefor *iter_x = head2;
    int candidate_present = 0;
    
    //creating temporary node
    struct votefor *temp = (struct votefor*)malloc(sizeof(struct votefor));
    char *data_name = (char *)malloc(sizeof(char));
    temp->data_name = malloc(100);
    strcpy(temp->data_name,name);
    temp->data_count= 1;
    temp->next_votefor=NULL;
    
    for( iter_x = head2; iter_x != NULL ; iter_x = iter_x->next_votefor)
    {
        if (strcmp(iter_x->data_name,temp->data_name) == 0) {
            candidate_present =1;
            iter_x->data_count = iter_x->data_count +1;
            
            fflush(stdout);
            return EXISTS; //3
        }
    }
    if (candidate_present == 0) {
        
        struct votefor *newCand = (struct votefor*)malloc(sizeof(struct votefor));
        char *data_name = (char *)malloc(sizeof(char));
        newCand->data_name = malloc(100);
        strcpy(newCand->data_name,name);
        newCand->data_count= 1;
        newCand->next_votefor=NULL;
        
        
        if (forhead == 0) {
            head2 = newCand;
            nxtptr = newCand;
            forhead = 1;
        }else{
            nxtptr->next_votefor = newCand;
            nxtptr=nxtptr->next_votefor;
        }
        return NEWCAND; //8
        
    }
    return ERROR; //4
}


void votefor(){
    
    bzero(buf1_votefor,MAXBUFLEN);
    
    if((cand= recvfrom(sock, buf1_votefor, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1)
    {
        perror("Server-recvfrom() error");
        exit(1);
    }
    
    bzero(buf2_votefor,MAXBUFLEN);
    
    if((vid = recvfrom(sock, buf2_votefor, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1)
    {
        perror("Server-recvfrom() error");
        exit(1);
    }
    
    int id=0;
    id = atoi(buf2_votefor);
    
    struct addvoter *iter;
    
    for (iter = head1; iter !=NULL; iter = iter->next_addvoter)
    {
        if (iter->data_voterid == id)
        {
            if (iter->data_bool == 0)
            {
                int result = create(buf1_votefor);
                if ( result == 8)
                {
                    iter->data_bool = 1;
                    sendto(sock,"NEW",3,0,(const struct sockaddr *)&from,fromlen);
                    return;
                }
                else if ( result == 3)
                {
                    iter->data_bool = 1;
                    sendto(sock,"EXISTS",6,0,(const struct sockaddr *)&from,fromlen);
                    return;
                }
            }
            //if he has casted the vote
            else if( iter->data_bool == 1){
                sendto(sock,"ALREADYVOTED",12,0,(const struct sockaddr *)&from,fromlen);
                return;
            }
            
        }
    }
    sendto(sock,"NOTAVOTER",9,0,(const struct sockaddr *)&from,fromlen);
    return;
}

// LIST_CANDIDATES

void listcandidates(){
    
    struct votefor *iter = head2;
    bzero(listcand_buf,128);
    
    while (iter !=NULL) {
        strcat(listcand_buf,iter->data_name);
        iter = iter->next_votefor;
    }
   
    fflush(stdout);
    sendto(sock,listcand_buf,strlen(listcand_buf)+1,0,(const struct sockaddr *)&from,fromlen);
   
    return;
}


// VOTECOUNT

void count(char *cand_name){
    
struct votefor *iter;

for(iter = head2; iter !=NULL; iter = iter->next_votefor){
    if (strcmp(iter->data_name,cand_name) == 0) {
            
    int cnt = iter->data_count;
    snprintf(votecount_buf,sizeof(votecount_buf), "%d", cnt);

    fflush(stdout);
    sendto(sock,votecount_buf,strlen(votecount_buf)+1,0,(const struct sockaddr *)&from,fromlen);
    return;
        }
    }

    
}

void votecount(){
    
    if((bytes_votecount = recvfrom(sock, buf_votecount, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1) {
        perror("Server receive error ");
        exit(1);
    }
    else {
        count(buf_votecount);
    }
    
}

// VIEW_RESULT


void result_calculate(){
    
    struct votefor *iter = head2;
    int max = 0, tie = 0;
    
    char result[512],list_buf[512];
    
    if (iter == NULL) {
        sendto(sock,"NULL",4,0,(const struct sockaddr *)&from,fromlen);
        fflush(stdout);
    }
    
    //find max here
    while (iter != NULL) {
        if (iter->data_count > max) {
            max= iter->data_count;
            fflush(stdout);
        }
        iter = iter->next_votefor;
        
    }
    
    //tie logic
    iter = head2;
    while (iter != NULL) {
        
        if (iter->data_count == max) {
            strcat(result_buf,iter->data_name);
            tie = tie+1;
            strcat(result_buf," ");

        }
        iter = iter->next_votefor;
    }
    
    //print list of cand & their vote-count
    
    iter = head2;
    while (iter != NULL) {
        
        strcat(list_buf,iter->data_name);
        strcat(list_buf," ");
        
        //strcat(list_buf,iter->data_count);
        
        char vcount[512];
        int p = iter->data_count;
        sprintf(vcount,"%d",p);
        strcat(list_buf,vcount);
        
        strcat(list_buf,"\n");
        
        iter = iter->next_votefor;
    }
    
    
    //declare winner/tie
    if (tie >=2) {
        // tie-max-abc
        bzero(final_buf,512);
        
        strcpy(final_buf,"TIE");
        strcat(final_buf," ");
        
        strcat(final_buf,"winners: ");
        strcat(final_buf,result_buf);
        
        strcat(result,"\n");
        strcat(final_buf,"List: ");
        strcat(result,"\n");
        
        strcat(final_buf,list_buf);

        sendto(sock,final_buf,strlen(final_buf)+1,0,(const struct sockaddr *)&from,fromlen);
        fflush(stdout);
        return;


    }
    else{
        // winner-max-a
        bzero(final_buf,512);
        
        strcpy(final_buf,"WINNER");
        strcat(final_buf," ");
        
        strcat(final_buf,"winner: ");
        strcat(final_buf,result_buf);
        
        strcat(result,"\n");
        strcat(final_buf,"List: ");
        strcat(result,"\n");
        
        strcat(final_buf,list_buf);
        
        sendto(sock,final_buf,strlen(final_buf)+1,0,(const struct sockaddr *)&from,fromlen);
        fflush(stdout);
        return;
        
    }
    
}

void viewresult(){
    
    bzero(buf_vresult1,MAXBUFLEN);
    if((bytes_vresult1 = recvfrom(sock, buf_vresult1, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1){
        perror("Server-recvfrom() error");
        exit(1);
    }
    
    bzero(buf_vresult2,MAXBUFLEN);
    if((bytes_vresult2 = recvfrom(sock, buf_vresult2, MAXBUFLEN-1, 0, ((struct sockaddr *)&from), &fromlen)) == -1){
        perror("Server-recvfrom() error");
        exit(1);
    }
    
    c = strcmp(user,buf_vresult1);
    d = strcmp(pass,buf_vresult2);
    
    
    if (c == 0 &&  d == 0) {
        
        result_calculate();
        exit(0);
    
    }
    else if(c != 0 ||  d != 0) {
        
        fflush(stdout);
        sendto(sock,"UNAUTHORIZED",12,0,(const struct sockaddr *)&from,fromlen);
        
        return;
    }
    
    else{
    fflush(stdout);
    sendto(sock,"ERROR",5,0,(const struct sockaddr *)&from,fromlen);
    }
    
}

// end of view result

// ZEROIZE

void zeroize(){
    //initial state, with no candidates, no voters, and zero votes
    
    struct addvoter *iter1= head1;
    struct addvoter *curr1;
    while (iter1 !=NULL) {
        curr1 = iter1->next_addvoter;
        iter1->next_addvoter = NULL;
        free(iter1);
        iter1 = curr1;
    }
    
    struct votefor *iter2= head2;
    struct votefor *curr2;
    while (iter2 !=NULL) {
        curr2 = iter2->next_votefor;
        iter2->next_votefor = NULL;
        free(iter2);
        iter2 = curr2;
    }
    
    head1 = NULL;
    head2 = NULL;
    
    forhead = 0;

    fflush(stdout);
    sendto(sock,"TRUE",4,0,(const struct sockaddr *)&from,fromlen);

    return;
}


int main(int argc, char *argv[])
{
    
    if (argc < 2) {
        fprintf(stderr, "You didnt give the port number\n");
        exit(0);
    }
    
    sock=socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("Opening socket");
    length = sizeof(server);
    
    bzero(&server,length);
    
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(atoi(argv[1]));
    
    if (bind(sock,(struct sockaddr *)&server,length)<0)
        error("binding");
    fromlen = sizeof(struct sockaddr_in);
    
    while (1) {
        if( (n = recvfrom(sock, buf_bytes, MAXBUFLEN-1, 0,((struct sockaddr *)&from), &fromlen ) )== -1 ) {
            perror("Client ID receive error");
            exit(1);
        }
        else{
            
            int x;
            x = atoi(buf_bytes);
            
            switch (x) {
                case 1:
                    change_password();
                    break;
                case 2:
                    addvoter();
                    break;
                case 3:
                    votefor();
                    break;
                case 4:
                    listcandidates();
                    break;
                case 5:
                    votecount();
                    break;
                case 6:
                    viewresult();
                    break;
                case 7:
                    zeroize();
                    break;
                default:
                    break;
            }
        }
    }
    return 0;
}

