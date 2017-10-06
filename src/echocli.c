#include <unp.h>
#include <stdio.h>

int main(int argc, char **argv){
	int sockfd, n;
	char recvline[MAXLINE + 1], sendline[MAXLINE + 1];
	char msg[MAXLINE + 1];
    struct sockaddr_in servaddr;
    fd_set readfs;


    if (argc < 3){
        err_sys("Illegal usage: ./echocli <IP ADDRESS> <PORT>\n");
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        err_sys("Error Creating Socket");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons((int) strtol(argv[2], (char **)NULL, 10));

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) < 0){
        err_sys("Can't translate Network IPs");
    }

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
        err_sys("connect error");

    printf("Welcome to Echo Client. Type anything to get an echo reply back from server\n");
    
    struct sockaddr_in local_addr;
    int addr_size = sizeof(local_addr);
    getsockname(sockfd, (struct sockaddr *)&local_addr, &addr_size);
    int dis = 1;

    if (argc > 3){
        dis = (int)strtol(argv[3], NULL, 10);
    }
    sprintf(msg, "Echo client: connection established at port %d\n", ntohs(local_addr.sin_port));
    if (write(dis, msg, strlen(msg)) < 0){
        printf("error: could not write to parent descriptor\n");
    }
    int maxfd = max(sockfd, STDIN_FILENO);
    while(1){
        FD_ZERO(&readfs);
        FD_SET(sockfd, &readfs);
        FD_SET(STDIN_FILENO, &readfs);
        int status = select(maxfd+1, &readfs, NULL, NULL, NULL);
        if (status < 0)
            err_sys("Invalid Select");
        if (FD_ISSET(sockfd, &readfs)){
            if (Readline(sockfd, recvline, MAXLINE) == 0){
                printf("Server connection closed..\n");
                break;
            }
            Fputs(recvline, stdout);
        }
        else if (FD_ISSET(STDIN_FILENO, &readfs)){
            if (Readline(STDIN_FILENO, sendline, MAXLINE) == 0){
                printf("EOF encountered. Closing connection..\n");
                Shutdown(sockfd, SHUT_WR);
                FD_CLR(STDIN_FILENO, &readfs);
                continue;
            }
            else
                Write(sockfd, sendline, strlen(sendline));
        }
    }
    close(sockfd);
    sprintf(msg, "Echo client: connection terminated at port %d\n", ntohs(local_addr.sin_port));
    if (write(dis, msg, strlen(msg)) < 0){
        printf("error: could not write to parent descriptor\n");
    }
    return 0;

}
