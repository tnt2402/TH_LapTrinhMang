#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

typedef struct user
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char status[MAX_USERNAME_LENGTH];
} User;

typedef struct node
{
    User data;
    struct node* next;
} Node;

Node* createNode(User userData);
Node* insertAtBeginning(Node* head, User userData);
Node* insertAtEnd(Node* head, User userData);
void displayList(Node* head);
void freeList(Node* head);

User createUser(const char* username, const char* password, const char* status);
Node* addUser(Node* userList, User account);

#endif /* LINKED_LIST_H */