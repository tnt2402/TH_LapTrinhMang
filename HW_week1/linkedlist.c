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
    char status[MAX_USERNAME_LENGTH];
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
        printf("Username: %s, Password: %s, Status: %s\n", temp->data.username, temp->data.password, temp->data.status);
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

User createUser(const char* username, const char* password, const char* status)
{
    User newUser;
    strcpy(newUser.username, username);
    strcpy(newUser.password, password);
    strcpy(newUser.status, status);
    return newUser;
}

Node* addUser(Node* userList, User account) {
    return insertAtEnd(userList, account);
}

int userExists(Node* head, const char* username)
{
    Node* temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->data.username, username) == 0)
        {
            return 1;  // User exists
        }
        
        temp = temp->next;
    }
  
    return 0;  // User does not exist
}