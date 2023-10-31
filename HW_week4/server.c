#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "linkedlist.h"
#include <ctype.h>

#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
Node *userList;
const char *code;
Acc currentUser;
const char *data;
#define MAXLINE 1000

void setStatusLogin(Acc tmp_user){
    currentUser = tmp_user;
    printf("You are now logged in.\n");
}

int isnhat(){
    return (strcmp(currentUser.username, "nhat") == 0 &&
            strcmp(currentUser.password, "nhat") == 0 &&
            currentUser.status == -1);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Cmd: ./server PortNumber\n");
        return 1;
    }
    currentUser = createUser("nhat", "nhat", -1, "");
    code = "20205111";
    userList = NULL;
    data = "nguoidung.txt";

    FILE *file;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char homepage[MAX_HOMEPAGE_LENGTH];

    int status;

    file = fopen(data, "r");
    if (file == NULL)
    {
        printf("Failed to open the file.");
        return 0;
    }

    while (fscanf(file, "%s %s %d %s", username, password, &status, homepage) == 4)
    {
        Acc tmp_user = createUser(username, password, status, homepage);

        userList = addUser(userList, tmp_user);
    }

    fclose(file);;

    int PORT = atoi(argv[1]);

    char buff[100];
    int listenfd;
    socklen_t len;
    struct sockaddr_in serveraddress, clientaddress;
    memset(&serveraddress, 0, sizeof(serveraddress));

    
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddress.sin_port = htons(PORT);
    serveraddress.sin_family = AF_INET;

    
    bind(listenfd, (struct sockaddr *)&serveraddress, sizeof(serveraddress));

    printf("Server listening on port %d\n", PORT);

    
    len = sizeof(clientaddress);
    char msg[MAXLINE];

    int count = 0;
    int n;
    while (1){
        if (!isnhat()){

            n = recvfrom(listenfd, buff, sizeof(buff), 0, (struct sockaddr *)&clientaddress, &len); // Receive message from client
            buff[n] = '\0';
            strcpy(password, buff);

            if (strcmp(password, "bye") == 0){
                strcpy(msg, "Goodbye!");
                sendto(listenfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddress, sizeof(clientaddress));
                strcpy(currentUser.username, "nhat");
                strcpy(currentUser.password, "nhat");
                currentUser.status = -1;
                continue;
            } else {
                // int SpecialChar = 0;
                // char charr[1024] = ""; 
                // char number[1024] = "";
                
                // for (int i = 0; i < strlen(password); i++){
                //     if ((password[i] >= 'a' && password[i] <= 'z') || (password[i] >= 'A' && password[i] <= 'Z')){
                //         strncat(charr, &password[i], 1);
                //     } else if (isdigit(password[i])){
                //         strncat(number, &password[i], 1);
                //     } else {
                //         SpecialChar = 1;
                //         break;
                //     }
                // }

                // if (SpecialChar){
                //     strcpy(msg, "Error password.\n");
                //     printf("%s\n", msg);
                //     sendto(listenfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddress, sizeof(clientaddress));
                //     continue;
                // } else {
                //     strcpy(msg, charr);
                //     strcat(msg, "\n");
                //     strcat(msg, number);
                    
                //     sendto(listenfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddress, sizeof(clientaddress));

                //     strcpy(currentUser.password, password);
                //     updatePassword(userList, currentUser);

                //     printf("Password is changed.\n");
                    
                //     FILE *fp = fopen(data, "w");
                //     if (fp == NULL){
                //         printf("Error opening file %s for writing.\n", data);
                //         return 0;
                //     }

                //     Node *temp = userList;
                    
                //     while (temp != NULL){
                //         fprintf(fp, "%s %s %d %s\n", temp->data.username, temp->data.password, temp->data.status, temp->data.homepage);
                //         temp = temp->next;
                //     }

                //     fclose(fp);
                //     continue;
                // }
                strcpy(currentUser.password, password);
                updatePassword(userList, currentUser);
                printf("Password is changed.\n");
                continue;
            }

        len = sizeof(clientaddress);
        int n = recvfrom(listenfd, buff, sizeof(buff), 0, (struct sockaddr *)&clientaddress, &len);
        buff[n] = '\0';
        strcpy(username, buff);
        n = recvfrom(listenfd, buff, sizeof(buff), 0, (struct sockaddr *)&clientaddress, &len);
        buff[n] = '\0';
        strcpy(password, buff);

        for (int i = 0; i < strlen(username); i++){
            if (username[i] == ' '){
                printf("Username can not contain space. Please enter username again.\n");
                continue;
            }
        }

        for (int i = 0; i < strlen(password); i++)
        {
            if (password[i] == ' ')
            {
                printf("Password cannot contain space. Please enter password again.\n");
                continue;
            }
        }

        Acc tmp_user = createUser(username, password, -1, "");
        int login_status = userExists(userList, &tmp_user);
        if (count == 3)
        {
            if (login_status == 1)
                printf("Account is blocked\n");
            block(userList, tmp_user);
            strcpy(msg, "Account is blocked");
            sendto(listenfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddress, sizeof(clientaddress));
            continue;
        }

        if (tmp_user.status == 0)
        {
            strcpy(msg, "Account is blocked");
            sendto(listenfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddress, sizeof(clientaddress));
            printf("User %s is locked.\n", tmp_user.username);
            printf("Contact administration for unlocking the user\n");
            continue;
        }

        if (login_status == 1)
        {
            strcpy(msg, "OK");
            sendto(listenfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddress, sizeof(clientaddress));
            printf("User %s successfully signed in.\n", username);
            setStatusLogin(tmp_user);
            continue;
        }
        else
        {
            strcpy(msg, "not OK");
            sendto(listenfd, msg, strlen(msg), 0, (struct sockaddr *)&clientaddress, sizeof(clientaddress));
            printf("Invalid username/password.\n");
        }
        count++;
    }
    }
    return 0;
}