#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

// Struct definitions

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

// Function to create a new node
Node* createNode(User userData)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    newNode->data = userData;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a node at the beginning of the list
Node* insertAtBeginning(Node* head, User userData)
{
    Node* newNode = createNode(userData);
    if (newNode == NULL)
    {
        return head;
    }

    newNode->next = head;
    return newNode;
}

// Function to insert a node at the end of the list
Node* insertAtEnd(Node* head, User userData)
{
    Node* newNode = createNode(userData);
    if (newNode == NULL)
    {
        return head;
    }

    if (head == NULL)
    {
        return newNode;
    }

    Node* temp = head;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = newNode;
    return head;
}

// Function to display the contents of the list
void displayList(Node* head)
{
    if (head == NULL)
    {
        printf("List is empty.\n");
        return;
    }

    Node* temp = head;
    while (temp != NULL)
    {
        printf("Username: %s, Password: %s, Status: %d\n", temp->data.username, temp->data.password, temp->data.status);
        temp = temp->next;
    }
}

// Function to free the memory occupied by the list
void freeList(Node* head)
{
    Node* temp;
    while (head != NULL)
    {
        temp = head->next;
        free(head);
        head = temp;
    }
}

User createUser(const char* username, const char* password, int status)
{
    User newUser;
    strcpy(newUser.username, username);
    strcpy(newUser.password, password);
    newUser.status = status;
    return newUser;
}

Node* addUser(Node* userList, User account) {
    return insertAtEnd(userList, account);
}

int userExists(Node* head, User* tmp_user)
{
    Node* temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->data.username, tmp_user->username) == 0)
        {
            tmp_user->status = temp->data.status;

            if (strcmp(temp->data.password, tmp_user->password) == 0) {
                return 1; // Username and password are correct
            } else {
                return -1; // Wrong password
            }
        }
        
        temp = temp->next;
    }
  
    return 0;  // User does not exist
}

void active(Node* head, User tmp_user) {
    Node* temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->data.username, tmp_user.username) == 0 && strcmp(temp->data.password, tmp_user.password) == 0)
        {
            temp->data.status = 1;
            return;  
        }
        
        temp = temp->next;
    }
  
    return;  
}

void block(Node* head, User tmp_user) {
    Node* temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->data.username, tmp_user.username) == 0)
        {
            temp->data.status = 0 ;
            return;  
        }
        
        temp = temp->next;
    }
  
    return;  
}

void updatePassword(Node* head, User tmp_user) {
    Node* temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->data.username, tmp_user.username) == 0)
        {
            strcpy(temp->data.password, tmp_user.password);
            return; 
        }
        
        temp = temp->next;
    }
  
    return;  
}