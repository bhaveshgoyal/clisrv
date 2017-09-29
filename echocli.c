#include <unp.h>
#include <stdio.h>

int main(int argc, char **argv){
	int sockfd, n;
	char recvline[MAXLINE+1];
	struct sockaddr_in servaddr;

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

	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
			recvline[n] = 0;        /* null terminate */
			if (fputs(recvline, stdout) == EOF)
					err_sys("fputs error");
	}
	if (n < 0)
			err_sys("read error");

	exit(0);

}
