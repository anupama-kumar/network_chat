#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TRUE   1
#define FALSE  0
#define PORT 2866

#define MAXBUFLEN 128

#define WRONG 1
#define RIGHT 2
#define EXISTS 3
#define ERROR 4
#define NEW 5
#define NOTAVOTER 6
#define ALREADYVOTED 7
#define NEWCAND 8

// sockets global

int opt = TRUE;
int main_sfd , addrlen , new_socket , client_socket[30];
int max_clients = 30 , numberofbytes, i , socket_file_desc;
int num_fd;

struct sockaddr_in address;

// sockets global

// to do
/*
readme - add that client arg for port should be 2866
*/

char user[128] = "cis505";
char pass[128] = "project2";

int n;
int forhead = 0;

char buf_main[MAXBUFLEN],buf1[MAXBUFLEN], buf2[MAXBUFLEN], buf3[MAXBUFLEN];
char final_buf[MAXBUFLEN],list_buf[MAXBUFLEN],result[MAXBUFLEN];

char votecount_buf[128], listcand_buf[128],result_buf[128];

int a,b;
int c,d;

int p;

// structs

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

// struct

// CHANGE_PASSWORD

void change_password(char buf1[],char buf2[],char buf3[]){
    
    a = strcmp(user,buf1);
    b = strcmp(pass,buf2);
    
    if (a == 0 &&  b == 0) {
        
        strcpy(pass,buf3);
        
        write(socket_file_desc,"TRUE",4);
        fflush(stdout);
        return;
    }
    else if ( a !=0 || b !=0){
        
        write(socket_file_desc,"FALSE",5);
        fflush(stdout);
        
        return;
    }

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
       
        write(socket_file_desc,"OK",128);
        fflush(stdout);
        
        return;
    }
    else if(head1 !=NULL){
        
        while (iter1 != NULL) {
            if (iter1->data_voterid == voterid) {
                
                write(socket_file_desc,"EXISTS",128);
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
        
        write(socket_file_desc,"OK",128);
        fflush(stdout);
        
        return;
        
    }
    else {
        write(socket_file_desc,"ERROR",128);
        fflush(stdout);
        
        return;
    }
    
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


void votefor(char temp[],int id){
    
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
                    write(socket_file_desc,"NEW",128);
                    return;
                }
                else if ( result == 3)
                {
                    iter->data_bool = 1;
                    
                    fflush(stdout);
                    write(socket_file_desc,"EXISTS",128);
                    
                    return;
                }
            }
            //if he has casted the vote
            else if( iter->data_bool == 1){
                
                fflush(stdout);
                write(socket_file_desc,"ALREADYVOTED",128);
                
                return;
            }

        }
    }
    for (iter = head1; iter !=NULL; iter = iter->next_addvoter)
    {
        if (iter->data_voterid != id)
        {
            fflush(stdout);
            write(socket_file_desc,"NOTAVOTER",128);
            return;
        }
        else{
            fflush(stdout);
            write(socket_file_desc,"ERROR",128);
            return;
        }
    }
    
}


// LISTCANDIDATE

void listcandidates(){
    
    struct votefor *iter_y = head2;
    bzero(listcand_buf,128);
    
    while (iter_y !=NULL) {
        strcat(listcand_buf,iter_y->data_name);
        iter_y = iter_y->next_votefor;
    }
    
    write(socket_file_desc,listcand_buf,strlen(listcand_buf)+1);
    fflush(stdout);
    return;
}



//VOTE_COUNT

void votecount(char temp_vc[]){
    
    struct votefor *iter;
    for(iter = head2; iter !=NULL; iter = iter->next_votefor){
        if (strcmp(iter->data_name,temp_vc) == 0) {
            
            int cnt = iter->data_count;
            
            snprintf(votecount_buf,sizeof(votecount_buf), "%d", cnt);
            
            write(socket_file_desc,votecount_buf,strlen(votecount_buf)+1);
            fflush(stdout);
            return;
        }
    }
    
    write(socket_file_desc,"-1",128);
    fflush(stdout);
    return;
}

// VIEW_RESULT


void viewresult(char t1[],char t2[]){
    
    
    c = strcmp(user,t1);
    d = strcmp(pass,t2);
    
    if (c == 0 &&  d == 0) {
        struct votefor *iter = head2;
        int max = 0;
        int tie = 0;
        
        if (iter == NULL) {
            write(socket_file_desc,"NULL",128);
            fflush(stdout);
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
            
            char vcount[128];
            int p = iter->data_count;
            sprintf(vcount,"%d",p);
            strcat(list_buf,vcount);
            
            strcat(list_buf,"\n");
            
            iter = iter->next_votefor;
        }
        
        
        //declare winner/tie
        if (tie >=2) {
            // tie-max-abc
            bzero(final_buf,128);
            
            strcpy(final_buf,"TIE");
            strcat(final_buf," ");
            
            strcat(final_buf,"winners: ");
            strcat(final_buf,result_buf);
            
            strcat(result,"\n");
            strcat(final_buf,"List: ");
            strcat(result,"\n");
            
            strcat(final_buf,list_buf);
            
            write(socket_file_desc,final_buf,strlen(final_buf)+1);
            fflush(stdout);
            
            
        }
        else{
            // winner-max-a
            bzero(final_buf,128);
            
            strcpy(final_buf,"WINNER");
            strcat(final_buf," ");
            
            strcat(final_buf,"winner: ");
            strcat(final_buf,result_buf);
            
            strcat(result,"\n");
            strcat(final_buf,"List: ");
            strcat(result,"\n");
            
            strcat(final_buf,list_buf);
            
            write(socket_file_desc,final_buf,strlen(final_buf)+1);
            fflush(stdout);
        }
        
    }
    
    else if(c!=0 || d !=0){
        write(socket_file_desc,"UNAUTHORIZED",128);
        fflush(stdout);
        return;
    }
    else {
        write(socket_file_desc,"ERROR",128);
        fflush(stdout);
        return;
    }
    exit(0);
}

// ZEROIZE

void zeroize(){
    
    struct addvoter *iter1= head1;
    struct addvoter *curr1;
    
    struct votefor *iter2= head2;
    struct votefor *curr2;
    
    while (iter1 !=NULL) {
        curr1 = iter1->next_addvoter;
        free(iter1);
        iter1 = curr1;
    }
    
    head1 = NULL;
    
    
    while (iter2 !=NULL) {
        curr2 = iter2->next_votefor;
        free(iter2);
        iter2 = curr2;
    }
    
    head2 = NULL;
    forhead = 0;
    
    write(socket_file_desc,"TRUE",4);
    fflush(stdout);
    return;
    
    
}



int main(int argc , char *argv[])
{
    char val[100];
    char temp[100];
    
    char old[100];
    char new[100];
    
    char user_result[100];
    char pwd_result[100];
    
    bzero(val,100);
    int idrecvd =0;
    
    char buffer[1025];
    
    fd_set readfds;
    for (i = 0; i < max_clients; i++){
        client_socket[i] = 0;
    }
    
    if( (main_sfd = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
        perror("socket has failed");
        exit(EXIT_FAILURE);
    }
    //this prevents reaed-write conflicts basically
    if( setsockopt(main_sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    if (bind(main_sfd, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);
    
    if (listen(main_sfd, 7) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    
    while(TRUE)
    {
        FD_ZERO(&readfds);
        FD_SET(main_sfd, &readfds);
        num_fd = main_sfd;
        
        for ( i = 0 ; i < max_clients ; i++)
        {
            socket_file_desc = client_socket[i];
            if(socket_file_desc > 0)
            {
             FD_SET( socket_file_desc , &readfds);
            }
            if(socket_file_desc > num_fd)
            {
                num_fd = socket_file_desc;
            }
        }
        
        numberofbytes = select( num_fd + 1 , &readfds , NULL , NULL , NULL);
        
        if ((numberofbytes < 0) && (errno!=EINTR)) {
            printf("select error");
        }
        
        if (FD_ISSET(main_sfd, &readfds))
        {
            if ((new_socket = accept(main_sfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            for (i = 0; i < max_clients; i++) {
                if( client_socket[i] == 0 ) {
                    client_socket[i] = new_socket;
                    
                    printf("This many clients are waiting: %d\n" , i);
                    
                    break;
                }
            }
        }
        
        for (i = 0; i < max_clients; i++)
        {
            socket_file_desc = client_socket[i];
            
            if (FD_ISSET( socket_file_desc , &readfds))
            {
                bzero(buf_main,MAXBUFLEN);
                
                if( (n = read(socket_file_desc,buf_main,MAXBUFLEN)) < 0){
                    close( socket_file_desc );
                    client_socket[i] = 0;
                }
                
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
                    }//switch
                }//else
                
            }//if
        }//for
    }//while
            return 0;
}
