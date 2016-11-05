char* message1 = "EXISTS";
char* message2 = "NEW";
char* message3 ="NOTAVOTER";
char* message4 = "ALREADYVOTED";
char* message5 = "ERROR";


char conv[50];
char buf[255];
int ex;

bzero(buf,255);
bzero(candbuffer,255);
bzero(voterbuffer,255);
bzero(votecount1,255);
bzero(usercheck,255);
bzero(passcheck,255);


struct candidate* newCand = (struct candidate*) malloc(sizeof(struct candidate));
struct candidate* temp;

struct Node* temp1;
newCand->name = malloc(30);
strcpy(newCand->name,name);
newCand->count=0;

newCand->next = NULL;
ex=0;

if(head==NULL){
    printf("NOTAVOTER\n");
    send(listenfd, "NOTAVOTER",BUFSIZE,0);
    return;
}
else {
    temp1=head;
    while(temp1!=NULL){
        if(temp1->data==voterid && temp1->iscast==1){
            printf("ALREADYVOTED\n");
            send(listenfd, "ALREADYVOTED", 255,0);
            return;
        }
        else if(temp1->data==voterid && temp1->iscast==0){
            temp1->iscast=1;
            ex=1;
            send(listenfd, "Done", 10,0);
            break;
        }
        temp1=temp1->next;
    }
    if(ex==0){
        printf("ex: %d\n",ex);
        printf("NOTAVOTER\n");
        send(listenfd, "NOTAVOTER", 255,0);
        return;
    }
}
if(headcand==NULL){
    headcand = newCand;
    newCand->count=1;
    printf("count: %d\n", newCand->count);
    printf("NEW\n");
    send(listenfd, "NEW", BUFSIZE,0);
    return;
}
else{
    if(strcmp(headcand->name,name)==0){
        headcand->count++;
        printf("count: %d\n",headcand->count);
        printf("Exists\n");
        //send(listenfd, "EXISTS", 255,0);
        return;
    }
    else{
        temp=headcand;
        while(temp->next!=NULL){
            temp=temp->next;
            if(strcmp(temp->name,name)==0){
                temp->count++;
                printf("%d\n",temp->count);
                printf("EXISTS\n");
                send(listenfd, "EXISTS", 255,0);
                return;
            }
        }
        temp->next=newCand;
        newCand->count=1;
        printf("NEW\n");
        send(listenfd, "NEW", 255,0);
        return;	
    }		
}

return;
}