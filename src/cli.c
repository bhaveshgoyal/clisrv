#include <stdio.h>
#include <unp.h>
#include <string.h>
#include <stdlib.h>

#define COMM_LEN 100
void monitor_child(void *arg){
    int fd = *(int*)arg;
    ssize_t n;
    char buff[1024] = {0};
    while((n = read(fd, buff, sizeof(buff))) > 0)
    {
        if (write(STDOUT_FILENO, buff, n) < 0)
            printf("error writing to stdout");
        memset(buff, 0, sizeof(buff));
    }
    return;
}
int is_ip(char *ipstring){
    struct sockaddr_in sa;
    return (inet_pton(AF_INET, ipstring, &(sa.sin_addr)) != 0);
}
void sig_handler(int sig)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        printf("Child process %d terminated: SIGCHLD received\n", pid);
    return;
}
void  SIGhandler(int sig)
{
         printf("\nReceived a SIGUSR1");
              exit(0);
}
void print_err(char *s){
    printf("%s\n", s);
    exit(1);
}

int main(int argc, char **argv){

    char command[COMM_LEN];
    struct hostent *he;
    struct in_addr *inaddr, **addr_list;
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
            printf("Server Hostname: %s\n", he->h_name);
        }
    }
    else{
        if ((he = gethostbyname(argv[1])) == NULL)
            print_err("Could Not resolve hostname");
        else{
            addr_list = (struct in_addr **) he->h_addr_list;
            printf("Server IP Address: %s\n", inet_ntoa(*addr_list[0]));
        }
    }

    
    char * s = getenv("DISPLAY");
  //  setenv("DISPLAY", ":0.0", 1);
    s = getenv("DISPLAY");
    if (strlen(s) == 0)
        err_sys("DISPLAY variable not set. Did you use -Y to forward host env");
    printf("Pointing xterm DISPLAY: %s\n\n\n", s);
    
    while(1){
        
        printf("$> ");
        if (!fgets(command, COMM_LEN, stdin))
            print_err("Error: could Not take input from stdin");
        if (command[0] == '\n' || command[0] == 0x0){
            continue;
        }
        if (strcmp(command, "echo\n") == 0){
            int efd[2];
            if (pipe(efd) < 0)
                print_err("Error piping echo descriptors");
            printf("Initiating echo client in a seperate xterm window..\n");
            if ((pid = fork()) == 0){
                close(efd[0]);
                char dis[10];
                snprintf(dis, sizeof(dis), "%d", efd[1]);
                if (execlp("/usr/openwin/bin/xterm", "xterm", "-e", "./bin/echocli", argv[1], argv[2], dis, NULL) < 0)
                    print_err("Error: could not create echo client");
            }
            else{
                close(efd[1]);
                pthread_t tid;
                if (pthread_create(&tid, NULL,(void *) monitor_child, (void*)&efd[0]) < 0){
                    printf("Could not create echo child monitor service: thread err");
                    continue;                 
                }
                pthread_detach(tid);
                while (waitpid(-1, NULL, WNOHANG) > 0);
            }
        }
        else if (strcmp(command, "time\n") == 0){
            int tfd[2];
            if (pipe(tfd) < 0)
                print_err("Error piping time descriptors");
            printf("Initiating time client in a seperate xterm window..\n");
            if ((pid = fork()) == 0){
                close(tfd[0]);
                char dis[10];
                snprintf(dis, sizeof(dis), "%d", tfd[1]);
                if (execlp("/usr/openwin/bin/xterm", "xterm", "-e", "./bin/timecli", argv[1], argv[3], dis, NULL) < 0)
                    print_err("Error: could not create timeclient");
            }
            else{
                close(tfd[1]);
                pthread_t tid;
                if (pthread_create(&tid, NULL,(void *) monitor_child, (void*)&tfd[0]) < 0){
                    printf("Could not create time child monitor service: thread err");
                    continue;                 
                }
                pthread_detach(tid);
                while (waitpid(-1, NULL, WNOHANG) > 0);
            }
        }
        else if (strcmp(command, "quit\n") == 0){
            print_err("Bye!");
        }
        else
            printf("Error: unknown command: %s\n", command);
    }

    return 0;
}
