#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 1000

int initializeSocket(const char *server_ip, int server_port) {
    int nw_socket;
    struct sockaddr_in sv_addr;

    nw_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (nw_socket == -1) {
        perror("Socket create failed");
        exit(EXIT_FAILURE);
    }

    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &(sv_addr.sin_addr)) <= 0) {
        printf("\nInvalid address or address is not supported. Please try again. \n");
        exit(EXIT_FAILURE);
    }

    if (connect(nw_socket, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    return nw_socket;
}

void login(int nw_socket) {
    char username[MAX], password[MAX], bufr[MAX];
    int logined = 0;

    while (!logined) {
        printf("Account: ");
        scanf(" %[^\n]", username);
        printf("Password: ");
        scanf(" %[^\n]", password);

        send(nw_socket, username, strlen(username), 0);

        send(nw_socket, password, strlen(password), 0);

        ssize_t n = recv(nw_socket, bufr, sizeof(bufr), 0);
        bufr[n] = '\0';
        printf("[-] Server: %s", bufr);

        if (strcmp(bufr, "OK") == 0) {
            logined = 1;
            printf("\nLogin successfully. Enter new password (except \"bye\"): ");
        } else {
            printf("\nCannot login. Try again.\n");
        }
    }
}

void changePassword(int nw_socket) {
    char new_password[MAX], bufr[MAX];

    while (1) {
        scanf(" %[^\n]", new_password);

        send(nw_socket, new_password, strlen(new_password), 0);

        ssize_t n = recv(nw_socket, bufr, sizeof(bufr), 0);
        bufr[n] = '\0';
        printf("[-] Server:\n%s\n", bufr);

        if (strcasecmp(new_password, "bye") == 0) {
            exit(0);
        } else {
            printf("Change password again? ");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./client IPAddress PortNumber\n");
        return 1;
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int nw_socket = initializeSocket(server_ip, server_port);

    login(nw_socket);

    changePassword(nw_socket);

    close(nw_socket);

    return 0;
}