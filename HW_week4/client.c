#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLINE 1000

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./client IPAddress PortNumber\n");
        return 1;
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int network_socket;
    struct sockaddr_in server_address;

    // Create a socket
    network_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address details
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &(server_address.sin_addr)) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char username[MAXLINE], password[MAXLINE], new_password[MAXLINE];
    char buffer[MAXLINE];
    int login_success = 0;

    while (!login_success)
    {
        printf("Account: ");
        scanf(" %[^\n]", username);
        printf("Password: ");
        scanf(" %[^\n]", password);

        // Send username to server
        sendto(network_socket, username, strlen(username), 0, (struct sockaddr *)&server_address, sizeof(server_address));

        // Send password to server
        sendto(network_socket, password, strlen(password), 0, (struct sockaddr *)&server_address, sizeof(server_address));

        // Receive result from server
        ssize_t n = recvfrom(network_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        buffer[n] = '\0';
        printf("[+] Server: %s", buffer);

        // Check login response
        if (strcmp(buffer, "OK") == 0)
        {
            login_success = 1;
        }
        else
        {
            printf("\nLogin failed. Please try again.\n");
        }
    }

    while (1)
    {

        printf("\nLogin successful. Enter new password (except \"bye\"): ");
        scanf(" %[^\n]", new_password);
        sendto(network_socket, new_password, strlen(new_password), 0, (struct sockaddr *)&server_address, sizeof(server_address));

        // Receive result from server
        ssize_t n = recvfrom(network_socket, buffer, sizeof(buffer), 0, NULL, NULL);
        buffer[n] = '\0';
        printf("[+] Server: %s\n", buffer);
        if (strcasecmp(new_password, "bye") == 0)
        {
            exit(0);
        }
        
    }

    // Close the socket
    close(network_socket);

    return 0;
}