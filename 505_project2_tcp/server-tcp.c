
/* TCP server */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXBUFLEN 128

#define WRONG 1
#define RIGHT 2
#define EXISTS 3
#define ERROR 4
#define NEW 5
#define NOTAVOTER 6
#define ALREADYVOTED 7
#define NEWCAND 8

int sockfd, newsockfd, portno;
struct sockaddr_in serv_addr, cli_addr;

char user[128] = "cis505";
char pass[128] = "project2";

int n,n1,n2,n3;
int forhead = 0;

char buf_main[MAXBUFLEN],buf1[MAXBUFLEN], buf2[MAXBUFLEN], buf3[MAXBUFLEN];
char buf_addvoter[MAXBUFLEN];
char buf1_votefor[MAXBUFLEN],buf2_votefor[MAXBUFLEN];
char buf_votecount[MAXBUFLEN];
char buf_vresult1[MAXBUFLEN],buf_vresult2[MAXBUFLEN];

char votecount_buf[128], listcand_buf[128],result_buf[128];

char final_buf[512];

int bytes,numbytes1,numbytes2,numbytes3,bytes_vresult1,bytes_vresult2;
int bytes_addvoter,bytes_votecount;

int a,b;
int c,d;

int cand,vid;


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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// CHANGE_PASSWORD

void change_password(char buf1[],char buf2[],char buf3[]){
    
    a = strcmp(user,buf1);
    b = strcmp(pass,buf2);
    
    if (a == 0 &&  b == 0) {
        
        strcpy(pass,buf3);
        write(newsockfd,"TRUE",4);
        fflush(stdout);
        return;
    }
    else if ( a !=0 && b !=0){
        write(newsockfd,"FALSE",5);
        fflush(stdout);
        return;
    }
    write(newsockfd,"ERROR",5);
    fflush(stdout);
    return;
}

// ADD_VOTER

void addvoter(int voterid){
    
    struct addvoter *temp_addvoter = (struct addvoter*)malloc(sizeof(struct addvoter));
    temp_addvoter->data_voterid= voterid;
    temp_addvoter->data_bool=0;
    temp_addvoter->next_addvoter= NULL;
    
    struct addvoter *iter1= head1;
    
    if(head1== NULL){
        head1 = temp_addvoter;
        write(newsockfd,"OK",128);
        fflush(stdout);
        return;
    }
    while (iter1 != NULL) {
        if (iter1->data_voterid == voterid) {
            write(newsockfd,"EXISTS",128);
            fflush(stdout);
            return;
        }
        iter1 = iter1->next_addvoter;
    }
    
    iter1 = head1;
    while (iter1->next_addvoter !=NULL ) {
        iter1 = iter1->next_addvoter;
    }
    iter1->next_addvoter = temp_addvoter;
    write(newsockfd,"OK",128);
    fflush(stdout);
    return;
    
}


//VOTE_FOR
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
        
        // create new node
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


void votefor(char temp[],int idgot){
    
    
    int id= idgot;
    
    struct addvoter *iter;
    
    for (iter = head1; iter !=NULL; iter = iter->next_addvoter)
    {
        if (iter->data_voterid == id)
        {
            if (iter->data_bool == 0)
            {
                int result = create(temp);
                if ( result == 8)
                {
                    iter->data_bool = 1;
                    fflush(stdout);
                    write(newsockfd,"NEW",128);
                    return;
                }
                else if ( result == 3)
                {
                    iter->data_bool = 1;
                    fflush(stdout);
                    write(newsockfd,"EXISTS",128);
                    return;
                }
            }
            //if he has casted the vote
            else if( iter->data_bool == 1){
                fflush(stdout);
                write(newsockfd,"ALREADYVOTED",128);
                return;
            }
            
        }
    }
    fflush(stdout);
    write(newsockfd,"NOTAVOTER",128);
    return;
}

// LISTCANDIDATE

void listcandidates(){
    
    struct votefor *iter = head2;
    bzero(listcand_buf,128);
    
    while (iter !=NULL) {
            strcat(listcand_buf,iter->data_name);
            iter = iter->next_votefor;
    }
    
    fflush(stdout);
    write(newsockfd,listcand_buf,strlen(listcand_buf)+1);
    return;
}



//VOTE_COUNT

void votecount(char temp_vc[]){
    
    struct votefor *iter;
    for(iter = head2; iter !=NULL; iter = iter->next_votefor){
               if (strcmp(iter->data_name,temp_vc) == 0) {

                   int cnt = iter->data_count;
                   
                   snprintf(votecount_buf,sizeof(votecount_buf), "%d", cnt);
                   
                   write(newsockfd,votecount_buf,strlen(votecount_buf)+1);
                   return;
               }
           }
    write(newsockfd,"-1",128);
    return;
}

// VIEW_RESULT


void result_calculate(){
    
    struct votefor *iter = head2;
    int max = 0;
    
    if (iter == NULL) {
        
        fflush(stdout);
        write(newsockfd,"NULL",128);
        return;
    }
    
    while (iter != NULL) {
        if (iter->data_count > max) {
            max= iter->data_count;
        }
        iter = iter->next_votefor;
        
    }
    
    iter = head2;
    while (iter != NULL) {
        
        if (iter->data_count == max) {
            strcpy(result_buf,iter->data_name);
        }
        iter = iter->next_votefor;
    }

    fflush(stdout);
    write(newsockfd,result_buf,strlen(result_buf)+1);
    return;
    
}

void viewresult(char t1[],char t2[]){
    
    char result[512],list_buf[512];
    
    c = strcmp(user,t1);
    d = strcmp(pass,t2);
    
    if (c == 0 &&  d == 0) {
        //result_calculate();
        struct votefor *iter = head2;
        int max = 0;
        int tie = 0;
        
        if (iter == NULL) {
            
            fflush(stdout);
            write(newsockfd,"NULL",128);
            return;
        }
        
        //find max here
        while (iter != NULL) {
            if (iter->data_count > max) {
                max= iter->data_count;
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
            
            write(newsockfd,final_buf,strlen(final_buf)+1);
            fflush(stdout);
            
            
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
            
            write(newsockfd,final_buf,strlen(final_buf)+1);
            fflush(stdout);
        }
    }
    
    else if(c!=0 || d !=0){
        
        fflush(stdout);
        write(newsockfd,"UNAUTHORIZED",128);
        
        return;
    }
    else {
        write(newsockfd,"ERROR",128);
        fflush(stdout);
        return;
    }
    exit(0);
}

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
    head1 = NULL;
    
    struct votefor *iter2= head2;
    struct votefor *curr2;
    while (iter2 !=NULL) {
        curr2 = iter2->next_votefor;
        iter2->next_votefor = NULL;
        free(iter2);
        iter2 = curr2;
    }
    
    
    head2 = NULL;
    forhead = 0;
    
    fflush(stdout);
    write(newsockfd,"TRUE",15);
    
    return;

}


int main(int argc, char *argv[])
{
    
    char val[100];
    char temp[100];
    
    char old[100];
    char new[100];
    
    char user_result[100];
    char pwd_result[100];
    
    bzero(val,100);
    int idrecvd =0;
    
    socklen_t clilen;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    
    //prevent read write buffering
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (int[]){128}, sizeof(int));
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    
    //start

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        bzero(buf_main,MAXBUFLEN);
        
        n = read(newsockfd,buf_main,MAXBUFLEN-1);
        if (n < 0) error("ERROR reading from socket");

        
        else{
            
            strcpy(val,strtok(buf_main,"\%"));
            int x = atoi(val);

            switch (x) {
                
                case 1:
                    bzero(val,100);
                    strcpy(val,strtok(NULL,"\%"));
        
                    bzero(old,100);
                    strcpy(old,strtok(NULL,"\%"));
                    
                    bzero(new,100);
                    strcpy(new,strtok(NULL,"\%"));
                    
                    change_password(val,old,new);
                    break;
                
                case 2:
                    bzero(val,100);
                    strcpy(val,strtok(NULL,"\%"));
                    idrecvd = atoi(val);
                    addvoter(idrecvd);
                    break;
                
                case 3:
                    bzero(val,100);
                    strcpy(val,strtok(NULL,"\%"));
                    
                    bzero(temp,100);
                    strcpy(temp,strtok(NULL,"\%"));
                    idrecvd = atoi(temp);
                    
                    votefor(val,idrecvd);
                    break;
                    
                case 4:
                    listcandidates();
                    break;
                    
                case 5:
                    bzero(val,100);
                    strcpy(val,strtok(NULL,"\%"));
                    votecount(val);
                    break;
                    
                case 6:
                    bzero(user_result,100);
                    strcpy(user_result,strtok(NULL,"\%"));
                    
                    bzero(pwd_result,100);
                    strcpy(pwd_result,strtok(NULL,"\%"));
                    
                    viewresult(user_result,pwd_result);
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
