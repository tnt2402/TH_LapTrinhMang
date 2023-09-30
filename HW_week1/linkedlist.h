#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

typedef struct user
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int status;
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

User createUser(const char* username, const char* password, int status);
Node* addUser(Node* userList, User account);
int userExists(Node* headm, User* tmp_user);
void active(Node* head, User tmp_user);
void block(Node* head, User tmp_user);
void updatePassword(Node* head, User tmp_user);


#endif /* LINKED_LIST_H */