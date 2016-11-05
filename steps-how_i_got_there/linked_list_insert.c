#include<stdio.h>
#include<stdlib.h>
struct node *head=NULL;
struct node *end=NULL;

struct node{
    int voterid;
    struct node *next;
};

void addvoter(int voterid)
{
    struct node *temp = (struct node*)malloc(sizeof(struct node));
    temp->voterid=voterid;
    temp->next=NULL;
    
    if(head==NULL){
        head=temp;
        end=temp;
        return;
    }
    end->next=temp;
    end=temp;
}

int main() {
    addvoter(1);
    return 0;
}
