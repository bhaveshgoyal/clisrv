#include <unp.h>
#include <stdio.h>
#include <time.h>
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m\n\0"
#define MAX_CLIENTS 50
#define HOSTNAME_SIZE 20
void sigpipe_handler(int signal){
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0)
        printf("Server thread %d terminated: SIGPIPE received", pid);
    return;
}
void *echo_handler(void* fd){
    int echo_conn = *(int *)fd;

    char sendbuf[MAXLINE+1] = {0};
    char tempbuf[MAXLINE+1] = {0};
    char buf[MAXLINE+1];
    ssize_t n;
    struct sockaddr_in sockaddr;
    socklen_t len = sizeof(sockaddr);

    getpeername(echo_conn, (struct sockaddr*)&sockaddr, &len);
    char ip[HOSTNAME_SIZE];
    inet_ntop(AF_INET, &sockaddr.sin_addr, ip, sizeof(ip));
    
    while((n = read(echo_conn, buf, MAXLINE)) > 0){        
        buf[n] = '\0';
        strcat(sendbuf, ANSI_COLOR_CYAN);
        strncpy(tempbuf, buf, strlen(buf)-1); // Remove NULL from buf
        strcat(sendbuf, tempbuf);
        strcat(sendbuf, ANSI_COLOR_RESET);

        Write(echo_conn, sendbuf, strlen(sendbuf));

        memset(sendbuf, 0, sizeof(sendbuf));
        memset(tempbuf, 0, sizeof(tempbuf));
    }
    if (n == 0){
        printf("Echo Client %s termination: socket read returned 0\n", ip);
        fflush(stdout);
        close(echo_conn);
    }
    return 0;
}
void *time_handler(void *fd){
    int time_conn = *(int *)fd;
    time_t ticks;
    
    char buf[MAXLINE+1];
    struct sockaddr_in sockaddr;
    socklen_t len = sizeof(sockaddr);

    getpeername(time_conn, (struct sockaddr*)&sockaddr, &len);
    char ip[HOSTNAME_SIZE];
    inet_ntop(AF_INET, &sockaddr.sin_addr, ip, sizeof(ip));

    while(1){
        ticks = time(NULL);
        snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
        if (write(time_conn, buf, strlen(buf)) < 0){
            break;
        }
        sleep(5);
    }
    printf("Time Client %s termination: EPIPE error detected\n", ip);
    fflush(stdout);
    close(time_conn);
    return 0;
}
int main(int argc, char **argv){

    struct sockaddr_in echosaddr, timesaddr;
    int echo_master, time_master, echo_conn, time_conn;
    char buf[MAXLINE + 1];
    ssize_t n;
    int i = 0;

    fd_set readfs;


    if (argc < 3)
        err_sys("Illegal usage: server echoport timeport");

    if ((echo_master = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("Error Creating Echo Socket");
    if ((time_master = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("Error Creating Time Socket");

    bzero(&(echosaddr), sizeof(echosaddr));
    bzero(&(timesaddr), sizeof(timesaddr));

    echosaddr.sin_family = AF_INET;
    echosaddr.sin_port = htons((int) strtol(argv[1], (char **)NULL, 10));
    echosaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    timesaddr.sin_family = AF_INET;
    timesaddr.sin_port = htons((int) strtol(argv[2], (char **)NULL, 10));
    timesaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(echo_master, (SA *) &echosaddr, sizeof(echosaddr));
    Bind(time_master, (SA *) &timesaddr, sizeof(timesaddr));

    Listen(echo_master, LISTENQ);
    Listen(time_master, LISTENQ);

    printf("Waiting for incoming clients...\n");

    int maxfd = max(echo_master, time_master);
    int status;
    while(1){
        FD_ZERO(&readfs);
        FD_SET(echo_master, &readfs);
        FD_SET(time_master, &readfs);
        status = select(maxfd+1, &readfs, NULL, NULL, NULL);
        if (status < 0)
            err_sys("Invalid Socket Select");
        if (FD_ISSET(time_master, &readfs)){
            time_conn = Accept(time_master, (SA *)NULL, NULL);
            struct sockaddr_in sockaddr;
            socklen_t len = sizeof(sockaddr);

            getpeername(time_conn, (struct sockaddr*)&sockaddr, &len);
            char ip[HOSTNAME_SIZE];
            inet_ntop(AF_INET, &sockaddr.sin_addr, ip, sizeof(ip));
            printf("New Time client connection established: %s\n", ip);
            fflush(stdout);
            pthread_t tid;
            Signal(SIGPIPE, sigpipe_handler);
            if (pthread_create(&tid, NULL, time_handler, (void*)&time_conn) < 0){
                printf("Could Not Create time service thread");
                continue;
            }
            pthread_detach(tid);
        }
        if (FD_ISSET(echo_master, &readfs)){
            echo_conn = Accept(echo_master, (SA *)NULL, NULL);
            struct sockaddr_in sockaddr;
            socklen_t len = sizeof(sockaddr);

            getpeername(echo_conn, (struct sockaddr*)&sockaddr, &len);
            char ip[HOSTNAME_SIZE];
            inet_ntop(AF_INET, &sockaddr.sin_addr, ip, sizeof(ip));
            printf("New Echo client connection established: %s\n", ip);
            fflush(stdout);
            pthread_t tid;
            Signal(SIGPIPE, sigpipe_handler);
            if (pthread_create(&tid, NULL, echo_handler, (void*)&echo_conn) < 0){
                printf("Could Not Create echo service thread");
                continue;
            }
            pthread_detach(tid);
        }
    }

    return 0;
}
