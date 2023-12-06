#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>

#define BACKLOG 10
#define BUFF_SIZE 1024

/********************************User list declarations**************************/
typedef struct Linked_List
{
	char userName[30];
	char passWord[30];
	int status;
	int error;
	int login;
	struct Linked_List *next;
} node;

node *head = NULL;
node *loginNode[BACKLOG];

node *MakeNode(char *userName, char *passWord, int status);

node *AddNode(char *userName, char *passWord, int status, node *h);

node *Search(char *userName);
/*********************************************************************************/

char res[BUFF_SIZE];
char sendBuff[BUFF_SIZE], rcvBuff[BUFF_SIZE];
int check_login[BACKLOG];


/*******************************Socket Initialization******************************/

/* The cpyStr function copies the input string to output */
void cpyStr(char *in, char *out);

int receiveData(int s, char *buff, int size, int flags)
{
	int n;
	n = recv(s, buff, size, flags);
	if (n < 0)
		perror("Error: ");
	else
	{
		buff[n] = '\0';
		printf("\n[+] Client: %s\n", buff);
	}
	return n;
}

int sendData(int s, char *buff, int size, int flags)
{
	int n;
	n = send(s, buff, size, flags);
	if (n < 0)
		perror("Error: ");
	return n;
}


int encryptPass(char *buff, char *result, char *number);

void Write2DataFile();
int SignInUser();
void ChangePassword(char *newpass, int i);
void SignOut();
void ReadDataFromFile();


/********************Main Func********************/
int main(int argc, char *argv[])
{
	int i, maxi, maxfd, listenfd, connfd, sockfd, fdmax;
	int nready, client[FD_SETSIZE];
	ssize_t ret, ret1, ret2;
	fd_set readfds, allset;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	struct pollfd fds[BACKLOG];
	int sin_size = sizeof(cliaddr);

	for (int j = 0; j < BACKLOG; j++)
	{
		check_login[j] = 0;
		loginNode[j] = NULL;
	}

	if (argc != 2)
	{
        printf("Usage: ./server Port\n");
		exit(EXIT_FAILURE);
	}

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{ /* calls socket() */
		perror("\n[!]: ");
		return 0;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
	{ /* calls bind() */
		perror("\n[!]: ");
		return 0;
	}

	if (listen(listenfd, BACKLOG) == -1)
	{ /* calls listen() */
		perror("\n[!]: ");
		return 0;
	}

	printf("Server is running\n");

	ReadDataFromFile();
	struct pollfd pollfds[BACKLOG + 1];
	pollfds[0].fd = listenfd;
	pollfds[0].events = POLLIN | POLLPRI;
	int useClient = 0;
	typedef struct login
	{
		char username[BUFF_SIZE], password[BUFF_SIZE];
		char number[BUFF_SIZE], text[BUFF_SIZE];
		char result[BUFF_SIZE];
	}Account;
	
	Account acc[BACKLOG];
	for (int i = 1	; i < BACKLOG +1; i++)
	{
		memset(acc[i].username,'\0', BUFF_SIZE);
		memset(acc[i].password,'\0', BUFF_SIZE);
		memset(acc[i].number,'\0', BUFF_SIZE);
		memset(acc[i].text,'\0', BUFF_SIZE);
		memset(acc[i].result,'\0', BUFF_SIZE);
	}
	
	

	while (1)
	{
		printf("Client: %d\n", useClient);
		int pollResult = poll(pollfds, useClient + 1, 5000);
		if (pollResult > 0)
		{
			if (pollfds[0].revents & POLLIN)
			{
				// struct sockaddr_in cliaddr;
				int addrlen = sizeof(cliaddr);
				int client_socket = accept(listenfd, (struct sockaddr *)&cliaddr, &addrlen);
				char client_ip[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(cliaddr.sin_addr), client_ip, INET_ADDRSTRLEN);

				printf("Client [%d] accepted!\n", useClient+1);
				for (int i = 1; i < BACKLOG; i++)
				{
					if (pollfds[i].fd == 0)
					{

						pollfds[i].fd = client_socket;
						pollfds[i].events = POLLIN | POLLPRI;
						useClient++;
						break;
					}
				}
			}
			
			for (int i = 1; i < BACKLOG; i++)
			{
				if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
				{
					char buf[BUFF_SIZE];
					
					sockfd  =  pollfds[i].fd;
					
					int bufSize = read(sockfd, buf, BUFF_SIZE - 1);
					// int bufSize = recv(sockfd, buf, sizeof(buf), 0);
					if (bufSize == -1)
					{
					    pollfds[i].fd = 0;
					    pollfds[i].events = 0;
					    pollfds[i].revents = 0;
						memset(acc[i].username,'\0', BUFF_SIZE);
						memset(acc[i].password,'\0', BUFF_SIZE);
					    useClient--;
					}
					else if (bufSize == 0)
					{
						pollfds[i].fd = 0;
					    pollfds[i].events = 0;
					    pollfds[i].revents = 0;
						memset(acc[i].username,'\0', BUFF_SIZE);
						memset(acc[i].password,'\0', BUFF_SIZE);
					    useClient--;
					}
					else
					{
						
						buf[bufSize] = '\0';
						if (strlen(acc[i].username) == 0)
						{	
							strcpy(acc[i].username, buf);
							send(sockfd, "Input password", BUFF_SIZE, 0);
							printf("Client[%d] - username: %s\n",i, acc[i].username);
						}
						else{
							strcpy(acc[i].password, buf);
							printf("Client[%d] - Username & Password: %s %s\n",i, acc[i].username, acc[i].password);
						}

						if (check_login[i] == 0 && strlen(acc[i].username) != 0 && strlen(acc[i].password) != 0)
						{
							if (SignInUser(acc[i].username, acc[i].password,sockfd, i) == 1)
							{
								check_login[i] = 1;
							}
							else if(SignInUser(acc[i].username, acc[i].password,sockfd, i) == 0){
								memset(acc[i].username,'\0', BUFF_SIZE);
								memset(acc[i].password,'\0', BUFF_SIZE);
								continue;
							}
						}
						else if(check_login[i] == 1 && strlen(acc[i].password) != 0){
							int x = encryptPass(acc[i].password, acc[i].text, acc[i].number);
							if (x == 1)
							{
								send(sockfd, "Special Characters Found", BUFF_SIZE, 0);
							}
							else{
								strcpy(acc[i].result, acc[i].text);
								strcat(acc[i].result," ");
								strcat(acc[i].result, acc[i].number);
								send(sockfd, acc[i].result, BUFF_SIZE, 0);;
								ChangePassword(acc[i].password, i);
								Write2DataFile();
								memset(acc[i].text,'\0', BUFF_SIZE);
								memset(acc[i].number,'\0', BUFF_SIZE);
								memset(acc[i].result,'\0', BUFF_SIZE);
							}
							
						}
						
					}
					
				}
			}
		}
	}
	return 0;
}

void cpyStr(char *in, char *out)
{
	strcpy(out, in);
}

int encryptPass(char *buff, char *result, char *number)
{
	int length = strlen(buff);
	int l1 = 0;
	int l2 = 0;
	for (int i = 0; i < length; i++)
	{
		if (buff[i] >= '0' && buff[i] <= '9')
		{
			number[l2++] = buff[i];
			number[l2] = 0;
		}
		else if ((buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >= 'A' && buff[i] <= 'Z'))
		{
			result[l1++] = buff[i];
			result[l1] = 0;
		}
		else
			return 1;
	}
	return 0;
}

node *MakeNode(char *userName, char *passWord, int status)
{
	node *temp = (node *)malloc(sizeof(node));
	strcpy(temp->userName, userName);
	strcpy(temp->passWord, passWord);
	temp->status = status;
	temp->error = 0;
	temp->login = 0;
	temp->next = NULL;
	return temp;
}

node *AddNode(char *userName, char *passWord, int status, node *h)
{
	if (h == NULL)
	{
		h = MakeNode(userName, passWord, status);
		return h;
	}
	int c = strcmp(userName, h->userName);
	if (c > 0)
	{
		h->next = AddNode(userName, passWord, status, h->next);
		return h;
	}
	else if (c == 0)
		return h;
	else if (c < 0)
	{
		node *tmp = MakeNode(userName, passWord, status);
		tmp->next = h;
		return tmp;
	}
	return NULL;
}

node *Search(char *userName)
{
	for (node *h = head; h != NULL; h = h->next)
	{
		if (strcmp(userName, h->userName) == 0)
			return h;
	}
	return NULL;
}

void Write2DataFile()
{
	FILE *fp = fopen("account.txt", "w+");
	for (node *h = head; h != NULL; h = h->next)
	{
		fprintf(fp, "%s %s %d\n", h->userName, h->passWord, h->status);
	}
	fclose(fp);
}

int SignInUser(char *user, char *pass, int s, int i)
{
	node *find = Search(user);
	if (find == NULL)
	{
		printf("Cannot find account\n");
		strcpy(res, "Cannot find account");
		cpyStr(res, sendBuff);
		sendData(s, sendBuff, strlen(sendBuff), 0);
		return 0;
	}
	if (find->status == 0)
	{
		printf("Account is blocked\n");
		strcpy(res, "Account is blocked");
		cpyStr(res, sendBuff);
		sendData(s, sendBuff, strlen(sendBuff), 0);
		return 0;
	}
	int c = strcmp(pass, find->passWord);
	if (c != 0)
	{
		find->login++;
		if (find->login < 3)
		{
			printf("Password is incorrect\n");
			strcpy(res, "Password is incorrect");
			cpyStr(res, sendBuff);
			sendData(s, sendBuff, strlen(sendBuff), 0);
			return 0;
		}
		else
		{
			find->status = 0;
			printf("Password is incorrect. Account is blocked\n");
			strcpy(res, "Password is incorrect. Account is blocked");
			cpyStr(res, sendBuff);
			sendData(s, sendBuff, strlen(sendBuff), 0);
			Write2DataFile();
			return 0;
		}
	}
	else
	{
		if (find->status == 0)
		{
			printf("Account is blocked\n");
			strcpy(res, "Account is blocked");
			cpyStr(res, sendBuff);
			sendData(s, sendBuff, strlen(sendBuff), 0);
			return 0;
		}
		else
		{
			loginNode[i] = find;
			loginNode[i]->login = 0;
			strcpy(res, "OK");
			cpyStr(res, sendBuff);
			sendData(s, sendBuff, strlen(sendBuff), 0);
			printf("Hello %s\n", user);
			return 1;
		}
	}
}

void ChangePassword(char *newpass, int i)
{
	strcpy(loginNode[i]->passWord, newpass);
	printf("Password is changed\n");
	return;
}
void SignOut(char *user, int i)
{
	char userName[30];
	user[strlen(user) - 1] = '\0';

	strcpy(userName, user);
	node *find = Search(userName);
	if (find == NULL)
	{
		printf("Cannot find account\n");
		return;
	}
	int c = strcmp(userName, loginNode[i]->userName);
	if (c != 0)
	{
		printf("Username is incorrect\n");
		return;
	}
	else
	{
		loginNode[i] = NULL;
		printf("Goodbye %s\n", userName);
	}
}

void ReadDataFromFile()
{
	char userName[30];
	char passWord[30];
	int status;
	FILE *fp = fopen("account.txt", "r+");
	while (fscanf(fp, "%s %s %d", userName, passWord, &status) == 3)
	{
		head = AddNode(userName, passWord, status, head);
	}
	fclose(fp);
}
