#include <stdio.h>
#include <stdlib.h>

struct node
{
    int a;
    struct node *next;
};

void generate(struct node **, int);
void search(struct node *, int);

int main()
{
    struct node *head = NULL;
    int key, num;
    
    printf("\nEnter key to search: ");
    scanf("%d", &key);
    search(head, key);
    
    return 0;
}

void generate(struct node **head, int num)
{
    int i;
    struct node *temp;
    
    for (i = 0; i < num; i++)
    {
        temp = (struct node *)malloc(sizeof(struct node));
        temp->a = rand() % num;
        if (*head == NULL)
        {
            *head = temp;
            temp->next = NULL;
        }
        else
        {
            temp->next = *head;
            *head = temp;
        }
        printf("%d  ", temp->a);
    }
}

void search(struct node *head, int key)
{
    while (head != NULL)
    {
        if (head->a == key)
        {
            printf("key found\n");
            return;
        }
        head = head->next;
    }
    printf("Key not found\n");
}

