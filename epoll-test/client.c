#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE 32
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=5) {
		printf("Usage : %s <IP> <port> <m> <n>\n", argv[0]);
		exit(1);
	}
	
	srand(time(NULL));
	int i, j;

	// create M sockets for separately send the numbers.
	for (i=0; i<atoi(argv[3]); i++) {
		sock=socket(PF_INET, SOCK_STREAM, 0);   
		if(sock==-1)
			error_handling("socket() error");
		
		memset(&serv_adr, 0, sizeof(serv_adr));
		serv_adr.sin_family=AF_INET;
		serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
		serv_adr.sin_port=htons(atoi(argv[2]));
		
		if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
			error_handling("connect() error!");
		else
			puts("Connected...........");
		
		int po = 0;
		memset(message, 0, BUF_SIZE);

		// write numbers to socket
		for (j=0; j<atoi(argv[4]); j++) {
			int r = rand()%1024;
			// printf("random number: %d\n", r);
			po += snprintf(message+po, 20, "%d ", r);
			if (po > BUF_SIZE - 5) {
				write(sock, message, strlen(message));
				printf("writing to server: %s\n", message);
				memset(message, 0, BUF_SIZE);
				po = 0;
			}
		}

		printf("writing to server: %s\n", message);
		write(sock, message, strlen(message));
		close(sock);
	}

	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}