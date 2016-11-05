#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

char* uname[21];
char* pword[21];
int max_votes = 0;
char lcandidates[1000];

typedef struct candidate {
    char *name;
    int nVotes;
    struct candidate* next;
} candidate_t;

typedef struct voterid {
    int voterID;
    bool voted;
    struct voterid* next;
} voterID_t;

struct candidate* head_candidate = NULL;
struct voterid* head_voterid = NULL;


char* changepassword(void);
char* zeroize(void);
char* addvoter(int voterid);
char* votefor(char *name, int voterid);
void listcandidates(void);
int votecount(char *name);
char* viewresult(char *username, char *password);
char* parse(char* buf);

int main(int argc, char *argv[]){
    int sfd, cfd, port, n;
    socklen_t c_length;
    char buffer[100];
    char data[2048];
    struct sockaddr_in s_addr, c_addr;
    port = 6500;
    strcat(&uname[0],(char *)"karthikd\n");
    strcat(&pword[0],(char *)"Temp pass\n");
    if (argc != 1 && argc != 3 ) {
        
        printf("Error, incorrect number of arguments:%i\n",argc);
        exit(1);
    }
    
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sfd < 0){
        printf("Error, can't open socket\n");
        exit(1);
    }
    
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port);
    
    if (bind(sfd, (struct sockaddr *) &s_addr,sizeof(s_addr)) < 0){
        printf("Error, unable to bind\n");
        exit(1);
    }
    printf("Listening on port %i\n", port);
    listen(sfd,5);
    while(1){
        c_length = sizeof(c_addr);
        cfd = accept(sfd, (struct sockaddr *) &c_addr, &c_length);
        if (cfd < 0){
            printf("Error on accept\n");
        }
        memset(&buffer, 0, 100);
        n = read(cfd,buffer,99);
        if (n < 0)
            printf("Error, unable to read from socket\n");
        printf("In %s",buffer);
        memset(&data, 0, 2048);
        strcpy(data, parse(buffer, cfd));
        printf("Out:%s",data);
        if(strcmp(data, "listcandidates\n") == 0){
            n = write(cfd, lcandidates, strlen(lcandidates));
        }
        else if(strcmp(data, "off\n") == 0){
            n = write(cfd, lcandidates, strlen(lcandidates));
            //break;
        }
        else
            n = write(cfd, data, strlen(data));
        if (n < 0)
            printf("Error in writing to socket\n");
        close(cfd);
        
    }
    
    close(sfd);
    return 0;
    
}

char* parse(char* buf, int soc){
    char* str;
    char ret[50] = "Invalid command\n";
    const char s[2] = " ";
    str = strtok(buf, s);
    printf("InSTR: %s",str);
    if(strcmp(str, "changepassword\n") == 0){
        int temp = atoi(strtok(NULL, s));
        if(temp != 0)
            strcpy(ret, changepassword(temp, soc));
    }
    else if (strcmp(str, "addvoter") == 0){
        int temp = atoi(strtok(NULL, s));
        if(temp != 0)
            strcpy(ret, addvoter(temp));
    }
    else if (strcmp(str, "zeroize\n") == 0)
        strcpy(ret, zeroize());
    else if (strcmp(str, "votecount") == 0){
        str = strtok(NULL, s);
        int temp = votecount(str);
        sprintf(ret,  "%d", temp);
    }
    else if (strcmp(str, "votefor") == 0){
        str = strtok(NULL, s);
        if(str != NULL){
            char* str2 = strtok(NULL, s);
            if(str2 != NULL){
                int temp = atoi(str2);
                strcat(str,"\n");
                if(temp != 0 && strlen(str) < 11 && strlen(str) > 1)
                    strcpy(ret, votefor(str, temp));
            }
        }
    }
    else if (strcmp(str, "listcandidates") == 0){
        listcandidates();
        strcpy(ret, "listcandidates\n");
    }
    else
        printf("Invalid command\n");
    printf("OutSTR:%s", ret);
    return ret;
}

char* changepassword(void){
    char b[25];
    memset(b, "\n", 25);
    printf("Enter username\n");
    fgets(b,20,stdin);
    if(strcmp (b, uname) != 0){
        printf("Username is%s",uname);
        printf("Wrong username");
        return (char*)"FALSE\n";
    }
    printf("Enter password\n");
    memset(b, "\0", 25);
    fgets(b,20,stdin);
    if(strcmp (b, pword) != 0){
        printf("Password is%s",pword);
        printf("Wrong password");
        return (char*)"FALSE\n";
    }
    printf("Enter new password\n");
    memset(b, "\n", 25);
    fgets(b,20,stdin);
    memset(pword, "\n", sizeof(pword));
    strcpy(pword, b);
    printf("Password %s\n", pword);
    return (char*)"TRUE\n";
}


char* addvoter(int voterid){
    struct voterid* current_voterid;
    struct voterid* temp;
    if(head_voterid == NULL){
        head_voterid = malloc(sizeof(voterID_t));
        if(head_voterid == NULL)
            printf("Malloc fail");
        head_voterid->voterID = voterid;
        head_voterid->voted = false;
        head_voterid->next = NULL;
        return (char*)"OK\n";
    }
    else{
        current_voterid = head_voterid;
        while(current_voterid != NULL){
            if(current_voterid->voterID == voterid)
                return "EXISTS\n";
            if(current_voterid->next == NULL)
                break;
            current_voterid = current_voterid->next;
        }
        temp = malloc(sizeof(voterID_t));
        if(temp == NULL)
            printf("Malloc fail");
        temp->voterID = voterid;
        temp->voted = false;
        temp->next = NULL;
        current_voterid->next = temp;
        return (char*)"OK\n";
        
    }
}

char* votefor(char *name, int voterid){
    struct candidate* current_candidate;
    struct voterid* current_voterid;
    
    if(head_voterid != NULL){
        
        current_voterid = head_voterid;
        while(current_voterid != NULL){
            
            if(current_voterid->voterID == voterid){
                if(current_voterid->voted == true)
                    return (char *)"ALREADYVOTED\n";
                else{
                    current_voterid->voted = true;
                    if(head_candidate != NULL){
                        printf("String:%s",name);
                        current_candidate = head_candidate;
                        while(current_candidate != NULL){
                            char* string = current_candidate->name;
                            printf("String:%s",string);
                            if( strcmp(string, name) == 0){
                                current_candidate->nVotes++;
                                return (char *)"EXISTS\n";
                            }
                            if(current_candidate->next == NULL){
                                break;
                            }
                            current_candidate = current_candidate->next;
                        }
                        struct candidate* temp = malloc(sizeof(candidate_t));
                        if(temp == NULL)
                            printf("Malloc fail");
                        temp->name = strdup(name);
                        temp->nVotes = 1;
                        temp->next = NULL;
                        current_candidate->next = temp;
                        return (char*) "NEW\n";
                        
                    }
                    else{
                        head_candidate = malloc(sizeof(candidate_t));
                        if(head_candidate == NULL)
                            printf("Malloc fail");
                        head_candidate->name = strdup(name);
                        printf("N1%s",name);
                        printf("N2%s",head_candidate->name);
                        head_candidate->nVotes = 1;
                        head_candidate->next = NULL;
                        return (char*) "NEW\n";
                    }
                }
                
            }
            if(current_voterid->next == NULL)
                break;
            current_voterid = current_voterid->next;
        }
        return (char *)"NOTAVOTER\n";
    }
    else
        return (char *)"NOTAVOTER\n";
    
}

char* zeroize(void){
}

void listcandidates(void){
}

int votecount(char *name){
    printf("Name:%s", name);
    if(head_candidate != NULL){
        struct candidate* current_candidate = head_candidate;
        while(current_candidate != NULL){
            if( strcmp(current_candidate->name, name) == 0){
                return current_candidate->nVotes;
            }
            if(current_candidate->next != NULL)
                current_candidate = current_candidate->next;
            else
                break;
        }
    }
    return -1;
}

char* viewresult(char *username, char *password){
    return (char*) "off\n";
    
}