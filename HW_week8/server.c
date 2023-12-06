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

#define MAX_CLIENTS 10
#define USERNAME_LENGTH 20
#define PASSWORD_LENGTH 20

Node *userList;
const char *Code;
User currentUser;
const char *accountDataFile;
void* connectionHandler(void* socket_desc);
#define MAX 1000
int client_sockets[MAX_CLIENTS];
fd_set readfds;
int max_sd;


void get()
{
    currentUser = createUser("anonymous", "anonymous", -1, "");
    Code = "20205027";
    userList = NULL;
    accountDataFile = "nguoidung.txt";
}

void readDataFromFile()
{
    FILE *file;
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    char homepage[HOMEPAGE_LENGTH];

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

void writeDataToFile()
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



int isNull()
{
    return (strcmp(currentUser.username, "anonymous") == 0 &&
            strcmp(currentUser.password, "anonymous") == 0 &&
            currentUser.status == -1);
}

void *handleConnect(void *socket_desc)
{
    int connect_buf = *(int *)socket_desc;
    char msg[MAX];
    char username[MAX], password[MAX];
    int count = 0;

    while (1)
    {
        if (!isNull())
        {
            memset(password, 0, MAX);
            read(connect_buf, password, sizeof(password));
            password[strlen(password)] = '\0';

            if (strcmp(password, "bye") == 0)
            {
                strcpy(msg, "Seeya!");
                write(connect_buf, msg, strlen(msg));
                printf("User %s logged out\n", currentUser.username);
                strcpy(currentUser.username, "anonymous");
                strcpy(currentUser.password, "anonymous");
                currentUser.status = -1;
                break;
            }
            else
            {
                int hasSpecialChar = 0;
                char alb[1024] = "";
                char num[1024] = "";
                for (int i = 0; i < strlen(password); i++)
                {
                    if ((password[i] >= 'a' && password[i] <= 'z') || (password[i] >= 'A' && password[i] <= 'Z'))
                    {
                        strncat(alb, &password[i], 1);
                    }
                    else if (isdigit(password[i]))
                    {
                        strncat(num, &password[i], 1);
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
                    write(connect_buf, msg, strlen(msg));
                    continue;
                }
                else
                {
                    strcpy(msg, alb);
                    strcat(msg, "\n");
                    strcat(msg, num);
                    write(connect_buf, msg, strlen(msg));

                    printf("%s\n", password);

                    strcpy(currentUser.password, password);
                    updatePassword(userList, currentUser);

                    printf("Password is changed.\n");
                    writeDataToFile();

                    continue;
                }
            }
        }

        memset(username, 0, MAX);
        memset(password, 0, MAX);
        read(connect_buf, username, sizeof(username));
        read(connect_buf, password, sizeof(password));
        username[strlen(username)] = '\0';
        password[strlen(password)] = '\0';

        User temp = createUser(username, password, -1, "");
        int login_status = userExists(userList, &temp);

        if (count == 3)
        {
            if (login_status == 1)
                printf("Account is blocked\n");
            block(userList, temp);
            strcpy(msg, "Account is blocked");
            write(connect_buf, msg, strlen(msg));
            count = 0;
            continue;
        }

        if (temp.status == 0)
        {
            strcpy(msg, "Account is blocked");
            write(connect_buf, msg, strlen(msg));
            printf("User %s is locked.\n", temp.username);
            continue;
        }

        if (login_status == 1)
        {
            strcpy(msg, "OK");
            write(connect_buf, msg, strlen(msg));
            printf("User %s successfully signed in.\n", username);
            currentUser = temp;
            continue;
        }
        else
        {
            strcpy(msg, "not OK");
            write(connect_buf, msg, strlen(msg));
            printf("Invalid username/password.\n");
        }
        count++;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./server Port\n");
        return 1;
    }

    get();
    readDataFromFile();

    int PORT = atoi(argv[1]);
    int listen_buf;
    int client_sockets[MAX_CLIENTS];
    fd_set readfds;
    int max_sd;

    socklen_t length;
    struct sockaddr_in server_adress, client_adress;
    memset(&server_adress, 0, sizeof(server_adress));

    listen_buf = socket(AF_INET, SOCK_STREAM, 0);
    server_adress.sin_addr.s_addr = htonl(INADDR_ANY);
    server_adress.sin_addr.s_addr = INADDR_ANY;
    server_adress.sin_port = htons(PORT);

    bind(listen_buf, (struct sockaddr *)&server_adress, sizeof(server_adress));

    listen(listen_buf, 3);

    length = sizeof(client_adress);

    printf("Server is up and running on port %d\n", PORT);

    // pthread_t thread_id;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(listen_buf, &readfds);
        max_sd = listen_buf;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_sockets[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_sd)
                max_sd = sd;
        }

        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(listen_buf, &readfds))
        {
            int new_socket = accept(listen_buf, (struct sockaddr *)&client_adress, &length);

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int client_socket = client_sockets[i];
            if (FD_ISSET(client_socket, &readfds))
            {
                // Handle client connection using your existing handleConnect function
                handleConnect(&client_socket);

                // Close the socket and mark it as 0 in the array
                close(client_socket);
                client_sockets[i] = 0;
            }
        }
    }

    return 0;
}