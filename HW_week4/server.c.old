#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "linkedlist.h"

// global variables
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
Node *userList;
const char *predefinedCode;
User currentUser;
const char *accountDataFile;
#define MAXLINE 1000

void init()
{
    currentUser = createUser("anonymous", "anonymous", -1, "");
    predefinedCode = "20205027";
    userList = NULL;
    accountDataFile = "nguoidung.txt";
}

void readUserDataFromFile()
{
    FILE *file;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char homepage[MAX_HOMEPAGE_LENGTH];

    int status;

    file = fopen(accountDataFile, "r");
    if (file == NULL)
    {
        printf("Failed to open the file.");
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
    FILE *fp = fopen(accountDataFile, "w");
    if (fp == NULL)
    {
        printf("Error opening file %s for writing.\n", accountDataFile);
        return;
    }

    Node *temp = userList;
    while (temp != NULL)
    {
        fprintf(fp, "%s %s %d %s\n", temp->data.username, temp->data.password, temp->data.status, temp->data.homepage);
        temp = temp->next;
    }

    fclose(fp);
}

void setStatusLogin(User tmp_user)
{
    // set currentUser to tmp_user
    currentUser = tmp_user;

    printf("You are now logged in.\n");
}

int isAnonymous()
{
    return (strcmp(currentUser.username, "anonymous") == 0 &&
            strcmp(currentUser.password, "anonymous") == 0 &&
            currentUser.status == -1);
}

// Function to send message to the specified address
void sendMessage(int socket_fd, const char *message, struct sockaddr_in *dest_addr, socklen_t dest_len)
{
    sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)dest_addr, dest_len);
}

// Function to receive message from any client
void receiveMessage(int socket_fd, char *buffer, struct sockaddr *src_addr, socklen_t *src_len)
{
    ssize_t n = recvfrom(socket_fd, buffer, MAXLINE, 0, src_addr, src_len);
    buffer[n] = '\0';
    // return n;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./server PortNumber\n");
        return 1;
    }
    init();
    readUserDataFromFile();

    int PORT = atoi(argv[1]);

    char buffer[MAXLINE];
    int listenfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    // Create a UDP Socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    // Bind server address to socket descriptor
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("[+] Server listening on port %d\n", PORT);

    // Receive the datagram
    len = sizeof(cliaddr);

    char msg[MAXLINE];
    char username[MAXLINE], password[MAXLINE];

    int count = 0;
    // int n;

    while (1)
    {
        if (!isAnonymous())
        {
            // ask for new password
            receiveMessage(listenfd, buffer, (struct sockaddr *)&cliaddr, &len);
            strcpy(password, buffer);
            if (strcmp(password, "bye") == 0)
            {
                strcpy(msg, "Seeya!");
                sendMessage(listenfd, msg, &cliaddr, len);
                printf("User %s logged out\n", currentUser.username);
                strcpy(currentUser.username, "anonymous");
                strcpy(currentUser.password, "anonymous");
                currentUser.status = -1;
                // close(listenfd);
                continue;
            }
            else
            {
                int hasSpecialChar = 0;
                char alphabets[1024] = ""; // Initialize the strings with empty strings
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
                        // Found a special character
                        hasSpecialChar = 1;
                        break;
                    }
                }

                if (hasSpecialChar)
                {
                    strcpy(msg, "Error\n");
                    printf("%s\n", msg);
                    sendMessage(listenfd, msg, &cliaddr, len);
                    continue;
                }
                else
                {
                    // printf("Xâu chứa các ký tự chữ cái: %s\n", alphabets);
                    // printf("Xâu chứa các ký tự chữ số: %s\n", digits);
                    // strcpy(msg, "Password is changed.");
                    strcpy(msg, alphabets);
                    strcat(msg, "\n");
                    strcat(msg, digits);
                    sendMessage(listenfd, msg, &cliaddr, len);

                    strcpy(currentUser.password, password);
                    updatePassword(userList, currentUser);

                    printf("Password is changed.\n");
                    writeUserDataToFile();
                    continue;
                }
            }
        }
        len = sizeof(cliaddr);
        receiveMessage(listenfd, buffer, (struct sockaddr *)&cliaddr, &len);
        strcpy(username, buffer);
        receiveMessage(listenfd, buffer, (struct sockaddr *)&cliaddr, &len);
        strcpy(password, buffer);

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
            sendMessage(listenfd, msg, &cliaddr, len);
            count = 0;
            continue;
        }

        if (tmp_user.status == 0)
        {
            strcpy(msg, "Account is blocked");
            sendMessage(listenfd, msg, &cliaddr, len);
            printf("User %s is locked.\n", tmp_user.username);
            printf("Contact administration for unlocking the user\n");
            continue;
        }

        if (login_status == 1)
        {
            strcpy(msg, "OK");
            sendMessage(listenfd, msg, &cliaddr, len);
            printf("User %s successfully signed in.\n", username);
            setStatusLogin(tmp_user);
            continue;
        }
        else
        {
            strcpy(msg, "not OK");
            sendMessage(listenfd, msg, &cliaddr, len);
            printf("Invalid username/password.\n");
        }
        count++;
    }

    close(listenfd);

    return 0;
}