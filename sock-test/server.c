#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUF_SIZE 32

static void error_handling(char *buf);

// 实验基本的socket编程过程。
// socket 基本tcp stream通信分以下几个步骤：
//  -> socket() 
//  -> bind()
//  -> listen()
//  -> accept()
//  -> read/write()
int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_sz; 

	int str_len;

	char buf[BUF_SIZE];

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
    
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

    adr_sz=sizeof(clnt_adr);

    clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);

    // int flag = fcntl(clnt_sock,F_GETFL,0);
    // flag = flag&~O_NONBLOCK;

    // if (fcntl(clnt_sock, F_SETFL, flag) < 0)//设置为阻塞态
    //  {  
    //     perror("fcntl F_SETFL fail"); 
    //     close(clnt_sock);
    //     return -1; 
    //  }

    while (1) {
        memset(buf, 0, BUF_SIZE);
        str_len=read(clnt_sock, buf, BUF_SIZE-1);
        if (str_len == 0) {  // 客户端socket 关闭会导致此分支
            printf("closing connection\n");
            close(clnt_sock);
            break;
        } else if (str_len < 0 ) {
            if (errno == EAGAIN) break;
        } else {
            // 当str_len 恰好 === BUF_SIZE-1时，
            printf("-> (%d) %s\n", str_len, buf);
        }
    }

    return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}