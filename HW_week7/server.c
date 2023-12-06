#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include "linkedlist.h"

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
// #define MAX_HOMEPAGE_LENGTH 100
#define MAXLINE 1000

// Global variables
Node* userList;
const char* predefinedCode;
User currentUser;
const char* accountDataFile;

// Function prototypes
void init();
void readUserDataFromFile();
void writeUserDataToFile();
void setStatusLogin(User tmp_user);
int isAnonymous();
void* connectionHandler(void* socket_desc);
void sendMessage(int sockfd, const char* msg);
void receiveMessage(int sockfd, char* buffer);

void init()
{
    currentUser = createUser("anonymous", "anonymous", -1, "");
    predefinedCode = "20205027";
    userList = NULL;
    accountDataFile = "nguoidung.txt";
}

void readUserDataFromFile()
{
    FILE* file;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char homepage[MAX_HOMEPAGE_LENGTH];
    int status;

    file = fopen(accountDataFile, "r");
    if (file == NULL)
    {
        printf("Failed to open the file.");
        exit(1);
        return;
    }

    while (fscanf(file, "%s %s %d %s", username, password, &status, homepage) == 4)
    {
        User tmp_user = createUser(username, password, status, homepage);
        userList = addUser(userList, tmp_user);
    }

    fclose(file);
}

void writeUserDataToFile()
{
    FILE* fp = fopen(accountDataFile, "w");
    if (fp == NULL)
    {
        printf("Error opening file %s for writing.\n", accountDataFile);
        return;
    }

    Node* temp = userList;
    while (temp != NULL)
    {
        fprintf(fp, "%s %s %d %s\n", temp->data.username, temp->data.password, temp->data.status, temp->data.homepage);
        temp = temp->next;
    }

    fclose(fp);
}

void setStatusLogin(User tmp_user)
{
    currentUser = tmp_user;
    printf("You are now logged in.\n");
}

int isAnonymous()
{
    return (
        strcmp(currentUser.username, "anonymous") == 0 &&
        strcmp(currentUser.password, "anonymous") == 0 &&
        currentUser.status == -1
    );
}

void sendMessage(int sockfd, const char* msg)
{
    write(sockfd, msg, strlen(msg));
}

void receiveMessage(int sockfd, char* buffer)
{
    memset(buffer, 0, MAXLINE);
    read(sockfd, buffer, sizeof(buffer));
    buffer[strlen(buffer) - 1] = '\0';
}

void* connectionHandler(void* socket_desc)
{
    int connfd = *(int*)socket_desc;
    char msg[MAXLINE];
    char username[MAXLINE], password[MAXLINE];
    int count = 0;

    while (1)
    {
        if (!isAnonymous())
        {
            receiveMessage(connfd, password);
            if (strcmp(password, "bye") == 0)
            {
                strcpy(msg, "Seeya!");
                sendMessage(connfd, msg);
                printf("User %s logged out\n", currentUser.username);
                strcpy(currentUser.username, "anonymous");
                strcpy(currentUser.password, "anonymous");
                currentUser.status = -1;
                close(connfd);
                return NULL;
            }
            else
            {
                int hasSpecialChar = 0;
                char alphabets[1024] = "";
                char digits[1024] = "";
                for (int i = 0; i < strlen(password); i++)
                {
                    if ((password[i] >= 'a' && password[i] <= 'z') || (password[i] >= 'A' && password[i] <= 'Z'))
                    {
                        strncat(alphabets, &password[i], 1);
                    }
                    else if (isdigit(password[i]))
                    {
                        strncat(digits, &password[i], 1);
                    }
                    else
                    {
                        hasSpecialChar = 1;
                        break;
                    }
                }

                if (hasSpecialChar)
                {
                    strcpy(msg, "Error\n");
                    printf("%s\n", msg);
                    sendMessage(connfd, msg);
                    continue;
                }
                else
                {
                    strcpy(msg, alphabets);
                    strcat(msg, "\n");
                    strcat(msg, digits);
                    sendMessage(connfd, msg);

                    strcpy(currentUser.password, password);
                    updatePassword(userList, currentUser);

                    printf("Password is changed.\n");
                    writeUserDataToFile();
                    continue;
                }
            }
        }

        receiveMessage(connfd, username);
        receiveMessage(connfd, password);

        for (int i = 0; i < strlen(username); i++)
        {
            if (username[i] == ' ')
            {
                printf("Username cannot contain spaces. Please enter a valid username.\n");
                continue;
            }
        }

        for (int i = 0; i < strlen(password); i++)
        {
            if (password[i] == ' ')
            {
                printf("Password cannot contain spaces. Please enter a valid password.\n");
                continue;
            }
        }

        User tmp_user = createUser(username, password, -1, "");
        int login_status = userExists(userList, &tmp_user);

        if (count == 3)
        {
            if (login_status == 1)
                printf("Account is blocked\n");
            block(userList, tmp_user);
            strcpy(msg, "Account is blocked");
            sendMessage(connfd, msg);
            count = 0;
            continue;
        }

        if (tmp_user.status == 0)
        {
            strcpy(msg, "Account is blocked");
            sendMessage(connfd, msg);
            printf("User %s is locked.\n", tmp_user.username);
            printf("Contact administration for unlocking the user\n");
            continue;
        }

        if (login_status == 1)
        {
            strcpy(msg, "OK");
            sendMessage(connfd, msg);
            printf("User %s successfully signed in.\n", username);
            setStatusLogin(tmp_user);
            continue;
        }
        else
        {
            strcpy(msg, "not OK");
            sendMessage(connfd, msg);
            printf("Invalid username/password.\n");
        }
        count++;
    }

    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./server PortNumber\n");
        return 1;
    }

    init();
    readUserDataFromFile();

    int PORT = atoi(argv[1]);
    // char buffer[MAXLINE];
    int listenfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    // Create a TCP Socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    // Bind server address to socket descriptor
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // Listen for incoming connections
    listen(listenfd, SOMAXCONN);

    printf("[+] Server listening on port %d\n", PORT);

    pthread_t thread_id;

    while (1)
    {
        // Accept a client connection
        len = sizeof(cliaddr);
        int* new_sock = malloc(1);
        *new_sock = accept(listenfd, (struct sockaddr*)&cliaddr, &len);

        if (pthread_create(&thread_id, NULL, connectionHandler, (void*)new_sock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }
    }

    close(listenfd);

    return 0;
}