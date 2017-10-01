#include <unp.h>
#include <stdio.h>
void handler(int sig){
    // Handled
    return;
}
int main(int argc, char **argv){
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;

	if (argc < 3){
		err_sys("Illegal usage: ./timecli <IP ADDRESS> <PORT>\n");
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

    Signal(SIGUSR1, handler);
    raise(SIGUSR1);
    while((n = read(sockfd, recvline, MAXLINE)) > 0){
      //  recvline[n] = 0;
        fputs(recvline, stdout);
    }
    printf("Server connection closed. Host went down\n");
    close(sockfd);
	fflush(stdout);
    return 0;

}
