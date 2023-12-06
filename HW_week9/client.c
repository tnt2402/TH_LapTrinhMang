#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024

int main(int argc, char* argv[]){
	int client_sock;
	char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr;
	int msg_len, bytes_sent, bytes_received;
	
	if(argc!=3){
		printf("Usage: ./client IPAddress PortNumber\n");
		exit(EXIT_FAILURE);
	}

	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));

	if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) <= 0) {
		perror("[!] Invalid Connection");
        close(client_sock);
        exit(EXIT_FAILURE);
	}
	
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\n[!] Can not connect to sever!");
		close(client_sock);
        exit(EXIT_FAILURE);
	}
		
    while (1)
    {
		printf("\nSend message: ");
        memset(buff, '\0', sizeof(buff));
        fgets(buff, BUFF_SIZE, stdin);

        if (buff[strlen(buff) - 1] == '\n') {
            buff[strlen(buff) - 1] = '\0';
        }

        if (strcmp(buff, "") == 0) {
            break;
        }
		

        msg_len = strlen(buff);
		
            
        bytes_sent = send(client_sock, buff, msg_len, 0);
        if(bytes_sent < 0)
            perror("\nError: ");
        
        bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
        if (bytes_received < 0)
                perror("\nError: ");
        else if (bytes_received == 0)
                printf("Connection closed.\n");
            
        buff[bytes_received] = '\0';
        printf("[+] Server: %s\n", buff);
    }
    		
	close(client_sock);
	return 0;
}