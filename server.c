#include <unp.h>
#include <stdio.h>
int main(int argc, char **argv){

    struct sockaddr_in servaddr;
    int sockfd, connfd;
    if (argc < 3)
        err_sys("Illegal usage: server echoport timeport");

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_sys("Error Creating Socket");
    bzero(&(servaddr), sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(3333);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(sockfd, LISTENQ);

    while(1){
        connfd = Accept(sockfd, (SA *) NULL, NULL);
        Write(connfd, "Hello Client", sizeof("Hello Client"));
        close(connfd);
    }

    return 0;
}
