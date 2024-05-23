#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>

// 程序中需要注意\0 也占用一个字节。
#define BUF_SIZE 16
#define EPOLL_SIZE 50
void error_handling(char *buf);

int sum = 0;
int max = 1<<16;

int main(int argc, char *argv[]) {
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	epfd=epoll_create(EPOLL_SIZE);
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	event.events=EPOLLIN;
	event.data.fd=serv_sock;	
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	while(1) {
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if(event_cnt==-1) {
			puts("epoll_wait() error");
			break;
		}

		// puts("return epoll_wait");
        // printf("return epoll_wait: %d\n", event_cnt);
		for(i=0; i<event_cnt; i++) {
			// printf("socket of ep_event: %d\n", ep_events[i].data.fd);
			if(ep_events[i].data.fd==serv_sock) {
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				event.events=EPOLLIN; // 水平触发方式
				// event.events=EPOLLIN|EPOLLET;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("connected client: %d \n", clnt_sock);
			}
			else {
				printf("starting to read data from socket.....................\n");
				int num = 0;
				while(1) { 
					// 本来水平触发方式下不需要while循环将 fd中等饿内容全部读出的，
					// 因为水平触发方式下，只要套接字中存在数据， epoll_wait 就会返回该事件。
					// 但是因为要读取的数据可能会有截断的情况，所以要读取完整的客户端写入内容。
					memset(buf, 0, BUF_SIZE);
					// 这里采用BLOCKING的方式读取内容，可能是有问题的：
					// 如果恰好上次读完了所有的内容，这次
					str_len=read(ep_events[i].data.fd, buf, BUF_SIZE); 
					if(str_len==0) {
						epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
						close(ep_events[i].data.fd);
						printf("closed client: %d \n", ep_events[i].data.fd);
						break;
					} else if (str_len == -1) { 
						printf("error happens: %d\n", errno);
						break;
					} else {
						int i;
						for (i=0; i<str_len; i++) {
							if (buf[i] == ' ') {
								sum += num;
								num = 0;
								sum %= max;
							} else if (buf[i]<'0' || buf[i]>'9'){
							} else {
								num = num*10+(buf[i]-'0');
							}
						}

						// 如果恰好str_len 就是buf的末尾，则会导致read block住。
						if (str_len < BUF_SIZE) {
							break;
						}
					}
				}
				printf("current sum ...............................: %d\n", sum);
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}

void error_handling(char *buf)
{
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}