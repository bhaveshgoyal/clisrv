#include <unp.h>
#include <stdio.h>

int main(int argc, char **argv){
	int sockfd, n;
	char recvline[MAXLINE + 1];
    char msg[MAXLINE + 1];
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

//    fp = fopen("/tmp/timeconn","w");
//    fputc('1', fp);
//    fclose(fp);
    struct sockaddr_in local_addr;
    int addr_size = sizeof(local_addr);
    getsockname(sockfd, (struct sockaddr *)&local_addr, &addr_size);
    int dis = (int)strtol(argv[3], NULL, 10);

    sprintf(msg, "Time client: connection established at port %d\n", ntohs(local_addr.sin_port));
    if (write(dis, msg, strlen(msg)) < 0){
        printf("error: could not write to parent descriptor\n");
    }
    while((n = read(sockfd, recvline, MAXLINE)) > 0){
      //  recvline[n] = 0;
        fputs(recvline, stdout);
   //     fputs(recvline, argv[3]);
    }
    printf("Server connection closed. Host went down\n");
    close(sockfd);
	fflush(stdout);
    sprintf(msg, "Time client: connection terminated at port %d\n", ntohs(local_addr.sin_port));
    if (write(dis, msg, strlen(msg)) < 0){
        printf("error: could not write to parent descriptor\n");
    }
//    fp = fopen("/tmp/timeconn","w");
//    fputc('0', fp);
//    fclose(fp);
    return 0;

}
