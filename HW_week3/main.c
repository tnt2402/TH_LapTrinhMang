#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "linkedlist.h"

// global variables
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
Node *userList;
const char *predefinedCode;
User currentUser;
const char *accountDataFile;

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

void printMenu()
{
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Activate\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change password\n");
    printf("6. Sign out\n");
    printf("7. Homepage with domain name\n");
    printf("8. Homepage with IP address\n");

    printf("Your choice (1-8, other to quit): ");
}

void registerUser()
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char homepage[MAX_HOMEPAGE_LENGTH];

    int status;

    printf("Username: ");
    scanf(" %[^\n]", username);
    printf("Password: ");
    scanf(" %[^\n]", password);
    printf("Homepage: ");
    scanf(" %[^\n]", homepage);

    // Check if username or password contains spaces
    int i;
    for (i = 0; i < strlen(username); i++)
    {
        if (username[i] == ' ')
        {
            printf("Username cannot contain spaces. Please enter a valid username.\n");
            return;
        }
    }
    for (i = 0; i < strlen(password); i++)
    {
        if (password[i] == ' ')
        {
            printf("Password cannot contain spaces. Please enter a valid password.\n");
            return;
        }
    }
    for (i = 0; i < strlen(homepage); i++)
    {
        if (homepage[i] == ' ')
        {
            printf("Homepage cannot contain spaces. Please enter a valid homepage.\n");
            return;
        }
    }

    status = 2;

    User tmp_user = createUser(username, password, status, homepage);
    if (userExists(userList, &tmp_user) == 1)
    {
        printf("Account existed\n");
    }
    else
    {
        userList = addUser(userList, tmp_user);
        printf("Succesfully registration. Activation required\n");
    }

}

void activeUser()
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char activeUserCode[MAX_USERNAME_LENGTH];

    int count = 0;
    int i;
    printf("Username: ");
    scanf(" %[^\n]", username);
    printf("Password: ");
    scanf(" %[^\n]", password);

    // Check if username or password contains spaces

    for (i = 0; i < strlen(username); i++)
    {
        if (username[i] == ' ')
        {
            printf("Username cannot contain spaces. Please enter a valid username.\n");
            return;
        }
    }
    for (i = 0; i < strlen(password); i++)
    {
        if (password[i] == ' ')
        {
            printf("Password cannot contain spaces. Please enter a valid password.\n");
            return;
        }
    }

    User tmp_user = createUser(username, password, -1, "");
    int login_status = userExists(userList, &tmp_user);

    if (login_status == 0)
    {
        printf("User %s is not exist.\n", tmp_user.username);
        return;
    }
    else if (login_status == -1)
    {
        printf("Wrong passsword !.\n");
        return;
    }

    while (1 == 1)
    {
        if (count == 5)
        {
            printf("Activation code is incorrect.\nAccount is blocked\n");
            block(userList, tmp_user);
            break;
        }
        printf("Code: ");
        scanf(" %[^\n]", activeUserCode);

        for (i = 0; i < strlen(activeUserCode); i++)
        {
            if (activeUserCode[i] == ' ')
            {
                printf("Activation code cannot contain spaces. Please enter a valid password.\n");
                return;
            }
        }

        // Convert activeUserCode to lowercase
        for (int i = 0; activeUserCode[i]; i++)
        {
            activeUserCode[i] = tolower(activeUserCode[i]);
        }

        // Compare activeUserCode with predefined variable in lowercase


        if (strcmp(activeUserCode, predefinedCode) == 0)
        {
            printf("Account is activated.\n");
            User tmp_user = createUser(username, password, -1, "");
            active(userList, tmp_user);
            return;
        }
        else if (count < 4)
        {           
           printf("Account is not activated.\n");
        }
        count++;
    }

}

void setStatusLogin(User tmp_user)
{
    // set currentUser to tmp_user
    currentUser = tmp_user;

    printf("You are now logged in.\n");
}

void signIn()
{
    int count = 0;
    char username[MAX_USERNAME_LENGTH];
    printf("Username: ");
    scanf(" %[^\n]", username);
    int i;
    for (i = 0; i < strlen(username); i++)
    {
        if (username[i] == ' ')
        {
            printf("Username cannot contain spaces. Please enter a valid username.\n");
            return;
        }
    }

    User tmp_user = createUser(username, "", -1, "");
    int login_status = userExists(userList, &tmp_user);

    if (login_status != -1)
    {
        printf("User %s is not exist.\n", tmp_user.username);
        return;
    }

    while (1 == 1)
    {
        char password[MAX_PASSWORD_LENGTH];
        printf("Password: ");
        scanf(" %[^\n]", password);

        // Check if username or password contains spaces

        for (i = 0; i < strlen(password); i++)
        {
            if (password[i] == ' ')
            {
                printf("Password cannot contain spaces. Please enter a valid password.\n");
                return;
            }
        }
        User tmp_user = createUser(username, password, -1, "");
        int login_status = userExists(userList, &tmp_user);
        if (count == 3)
        {
            if (login_status == 1)
                printf("Account is blocked\n");
            block(userList, tmp_user);
            break;
        }

        if (tmp_user.status == 0)
        {
            printf("User %s is locked.\n", tmp_user.username);
            printf("Contact administration for unlocking the user");
            break;
        }

        if (login_status == 1)
        {
            printf("User %s successfully signed in.\n", username);
            setStatusLogin(tmp_user);
            break;
        }
        else
        {
            printf("Incorrect password.\n");
        }
        count++;
    }
}

int isAnonymous()
{
    return (strcmp(currentUser.username, "anonymous") == 0 &&
            strcmp(currentUser.password, "anonymous") == 0 &&
            currentUser.status == -1);
}

void searchUser()
{
    if (isAnonymous())
    {
        printf("Account is not sign in\n");
    }
    else
    {
        char username[MAX_USERNAME_LENGTH];
        char password[MAX_PASSWORD_LENGTH];

        printf("Username: ");
        scanf(" %[^\n]", username);

        // Check if username or password contains spaces
        int i;
        for (i = 0; i < strlen(username); i++)
        {
            if (username[i] == ' ')
            {
                printf("Username cannot contain spaces. Please enter a valid username.\n");
                return;
            }
        }

        User tmp_user = createUser(username, password, -1, "");
        int search_status = userExists(userList, &tmp_user);
        if (search_status == 0)
        {
            printf("Cannot find account\n");
        }
        else
        {
            printf("User %s has been founded!", username);
            printf("\nUsername: %s\nStatus: %d\n", tmp_user.username, tmp_user.status);
        }
    }
}

void changePassword()
{
    if (isAnonymous())
    {
        printf("Account is not sign in\n");
    }
    else
    {
        char current_password[MAX_PASSWORD_LENGTH];
        char password[MAX_PASSWORD_LENGTH];

        printf("Enter your current password: ");
        scanf(" %[^\n]", current_password);
        printf("Enter your new password: ");
        scanf(" %[^\n]", password);
        // Check if username or password contains spaces
        int i;
        for (i = 0; i < strlen(current_password); i++)
        {
            if (current_password[i] == ' ')
            {
                printf("Username cannot contain spaces. Please enter a valid username.\n");
                return;
            }
        }
        for (i = 0; i < strlen(password); i++)
        {
            if (password[i] == ' ')
            {
                printf("Password cannot contain spaces. Please enter a valid password.\n");
                return;
            }
        }

        if (strcmp(current_password, currentUser.password) == 0)
        {
            strcpy(currentUser.password, password);
            updatePassword(userList, currentUser);
            printf("Password is changed.\n");
        }
        else
        {
            printf("Current password is incorrect. Please try again\n");
        }
    }
}

void signOut()
{
    if (isAnonymous())
    {
        printf("Account is not sign in\n");
    }
    else
    {
        printf("Goodbye %s\n", currentUser.username);

        strcpy(currentUser.username, "anonymous");
        strcpy(currentUser.password, "anonymous");
        currentUser.status = -1;
    }
}

void printHomepage(int type, char query[MAX_HOMEPAGE_LENGTH]) {

    printf("\nHomepage: %s\n", query);
    char modifiedQuery[30];
    if (strncasecmp(query, "https://", 8) == 0) {
        strcpy(modifiedQuery, query + 8); // Copy the substring after "https://" to modifiedQuery
    } else if (strncasecmp(query, "www.", 4) == 0) {
        strcpy(modifiedQuery, query + 4); // Copy the substring after "www." to modifiedQuery
    } else if (strncasecmp(query, "https://www.", 12) == 0) {
        strcpy(modifiedQuery, query + 12); // Copy the substring after "https://www." to modifiedQuery
    } else {
        strcpy(modifiedQuery, query); // No prefix detected, copy the original query
    }
    struct hostent *host;
    if (type == 1) {
        struct in_addr addr;
        if (inet_aton(modifiedQuery, &addr) == 0) {
            printf("Invalid IP address\n");
            return;
        }
        host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    } else {
        host = gethostbyname(modifiedQuery);
    }

    if (host == NULL) {
        printf("Not found information\n");
        return;
    }

    if (type == 1) {
        printf("Official name: %s\n", host->h_name);
        printf("Alias names:\n");
        char **name;
        for (name = host->h_aliases; *name != NULL; name++) {
            printf("%s\n", *name);
        }
    } else {
        printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
        printf("Alias IPs:\n");
        char **addr;
        for (addr = host->h_addr_list + 1; *addr != NULL; addr++) {
            printf("%s\n", inet_ntoa(*(struct in_addr*)*addr));
        }
    }
}
void homepageWithDomainName() {
    if (isAnonymous())
    {
        printf("Account is not sign in\n");
    }
    else
    {
        printHomepage(2, currentUser.homepage);
    }
}

void homepageWithIPAddress() {
    if (isAnonymous())
    {
        printf("Account is not sign in\n");
    }
    else
    {
        printHomepage(1, currentUser.homepage);
    }
}


int main()
{
    init();

    readUserDataFromFile();

    do
    {
        printMenu();

        // Read input as a string
        char input[10];
        scanf("%s", input);

        char *endptr;
        long int choice_int = strtol(input, &endptr, 10);
        if (endptr == input || *endptr != '\0')
        {
            exit(0);
        }

        switch (choice_int)
        {
        case 1:
            registerUser();
            break;
        case 2:
            activeUser();
            break;
        case 3:
            signIn();
            break;
        case 4:
            searchUser();
            break;
        case 5:
            changePassword();
            break;
        case 6:
            signOut();
            break;
        case 7:
            homepageWithDomainName();
            break;
        case 8:
            homepageWithIPAddress(); 
            break;
        case 9:
            displayList(userList);
            break;
        default:
            printf("Exiting the program.\n");
            exit(0);
        }
        printf("\n");
        writeUserDataToFile();

    } while (1);

    freeList(userList);

    return 0;
}
