#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"


// global variables

Node* userList = NULL;
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20

void readUserDataFromFile()
{
    FILE *file;
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char status[MAX_USERNAME_LENGTH];

    file = fopen("data.dat", "r");
    if (file == NULL)
    {
        printf("Failed to open the file.");
        return;
    }

    while (fscanf(file, "%s %s %s", username, password, status) == 3)
    {
        User tmp_user = createUser(username, password, status);

        userList= addUser(userList, tmp_user);
    }

    fclose(file);
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
    printf("Your choice (1-6, other to quit): ");
}

void registerUser() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char status[MAX_USERNAME_LENGTH];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    
    strcpy(status, "idle");

    User tmp_user = createUser(username, password, status);
    userList = addUser2Dat(userList, tmp_user);
    printf("User %s successfully registered.\n", username);
    printMenu();
}


void activeUser() {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char status[MAX_USERNAME_LENGTH];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    printf("Enter status: ");
    scanf("%s", status);
    User tmp_user = createUser(username, password, status);
    userList = addUser(userList, tmp_user);
    printf("User %s successfully registered.\n", username);
    printMenu();
}

int main()
{
    int choice;
    readUserDataFromFile();

    do
    {
        printMenu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            registerUser();
            break;
        case 2:
            activeUser();
            break;
        case 3:
            printf("Sign in functionality goes here.\n");
            break;
        case 4:
            printf("Search functionality goes here.\n");
            break;
        case 5:
            printf("Change password functionality goes here.\n");
            break;
        case 6:
            printf("Sign out functionality goes here.\n");
            break;
        case 7:
            displayList(userList);
            printMenu();
            break;
        default:
            printf("Exiting the program.\n");
            return 0;
        }
        printf("\n");
    } while (1);

    freeList(userList);

    return 0;
}
