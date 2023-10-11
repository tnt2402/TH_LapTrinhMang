#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./resolver <1 for IP/2 for domain> <query>\n");
        return 1;
    }

    int type = atoi(argv[1]);
    char *query = argv[2];

    if (type != 1 && type != 2) {
        printf("Wrong parameter\n");
        return 1;
    }

    struct hostent *host;
    if (type == 1) {
        struct in_addr addr;
        if (inet_aton(query, &addr) == 0) {
            printf("Invalid IP address\n");
            return 1;
        }
        host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    } else {
        host = gethostbyname(query);
    }

    if (host == NULL) {
        printf("Not found information\n");
        return 1;
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

    return 0;
}