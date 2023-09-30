#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./webinfo <web address>\n");
        return 1;
    }

    char *query = argv[1];
    struct hostent *host = gethostbyname(query);

    if (host == NULL) {
        printf("Not found information\n");
        return 1;
    }

    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
    printf("Alias IP:\n");
    char **addr;
    for (addr = host->h_addr_list + 1; *addr != NULL; addr++) {
        printf("%s\n", inet_ntoa(*(struct in_addr*)*addr));
    }

    return 0;
}