#include <stdio.h>
#include <unp.h>
#include <string.h>

#define COMM_LEN 100

int is_ip(char *ipstring){
    struct sockaddr_in sa;
    return (inet_pton(AF_INET, ipstring, &(sa.sin_addr)) != 0);
}
void sig_handler(int sig)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG) > 0))
        printf("Child process %d terminated: SIGCHLD received\n", pid);
    printf("$> ");
    return;
}
void print_err(char *s){
    printf("%s\n", s);
    exit(1);
}

int main(int argc, char **argv){

    char command[COMM_LEN];
    struct hostent *he;
    struct in_addr *inaddr;
    pid_t pid;
    Signal(SIGCHLD, sig_handler);
    if (argc < 4){
        print_err("Illegal usage: incorrect number of arguments");
    }

    if (is_ip(argv[1])){
        if (inet_pton(AF_INET, argv[1], &inaddr) == 0)
            print_err("Could not resolve ip");
        else{
            he = gethostbyaddr(&inaddr, sizeof(inaddr), AF_INET);
            printf("Host by addr: %s\n", he->h_name);
        }
    }
    else{
        if ((he = gethostbyname(argv[1])) == NULL)
            print_err("Could Not resolve hostname");
        else{
            printf("Host by name: %s\n", he->h_name);
        }
    }
    char * s = getenv("DISPLAY");
    setenv("DISPLAY", ":0.0", 1);
    s = getenv("DISPLAY");
    printf("Pointing xterm DISPLAY: %s\n\n", s);
    while(1){
        printf("$> ");
        if (scanf("%s", command) < 0)
            print_err("Error: could Not take input from stdin");
        if (command == "\n")
            continue;
        if (strcmp(command, "echo") == 0){
            printf("Initiating echo client in a seperate xterm window..\n");
            if ((pid = fork()) == 0){
                if (execlp("/usr/bin/xterm", "xterm", "-e", "./bin/echocli", argv[1], argv[2], NULL) < 0)
                    print_err("Error: could not create echo client");
            }
        }
        else if (strcmp(command, "time") == 0){
            printf("Initiating time client in a seperate xterm window..\n");
            if ((pid = fork()) == 0){
                if (execlp("/usr/bin/xterm", "xterm", "-e", "./bin/timecli", argv[1], argv[3], NULL) < 0)
                    print_err("Error: could not create timeclient");
            }
        }
        else if (strcmp(command, "quit") == 0){
            print_err("Bye!");
        }
        else
            printf("Error: unknown command: %s\n", command);
    }

    return 0;
}
