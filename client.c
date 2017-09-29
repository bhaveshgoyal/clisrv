#include <stdio.h>
#include <unp.h>
#include <string.h>

#define COMM_LEN 100

int is_ip(char *ipstring){
    struct sockaddr_in sa;
    return (inet_pton(AF_INET, ipstring, &(sa.sin_addr)) != 0);
}

void print_err(char *s){
    printf("%s\n", s);
    exit(1);
}

int main(int argc, char **argv){

    char command[COMM_LEN];
    struct hostent *he;
    struct in_addr *inaddr;

    if (argc < 3){
        print_err("Illegal usage: incorrect number of arguments");
    }

    if (!is_ip(argv[1])){
        if ((he = gethostbyname(argv[1])) == NULL)
            print_err("Could Not resolve hostname");
        else{
            printf("Host by name: %s\n", he->h_name);
        }
    }
    else
        if (inet_pton(AF_INET, argv[1], &inaddr) == 0)
            print_err("Could not resolve ip");
        else{
            he = gethostbyaddr(&inaddr, sizeof(inaddr), AF_INET);
            printf("Host by addr: %s\n", he->h_name);
        }

    while(1){
        scanf("%s", command);
        if (strcmp(command, "echo") == 0)
            printf("Echoing\n");
        else if (strcmp(command, "time") == 0)
            printf("Timing\n");
        else if (strcmp(command, "quit") == 0){
            print_err("Quiting Parent..");
        }
        else
            printf("Unknown Command: %s\n", command);
    }

    return 0;
}
