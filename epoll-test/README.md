# epoll test

This programs contain server and client.
They are together for demonstrating epoll working model and programming process.

Client sends M groups of N numbers within M sockets.
Server receives them and add them together.

```shell
$ ./server 8080
return epoll_wait: 1
socket of ep_event: 3
connected client: 5
return epoll_wait: 1
socket of ep_event: 5
echo back data(12): 785 245 684
current sum: 1714
return epoll_wait: 1
socket of ep_event: 5
closed client: 5
return epoll_wait: 1
socket of ep_event: 3
connected client: 5
return epoll_wait: 1
socket of ep_event: 5
echo back data(12): 720 507 641
current sum: 3582
return epoll_wait: 1
socket of ep_event: 5
closed client: 5

```shell
$ ./client 127.0.0.1 8080 2 3
Connected...........
random number: 785
random number: 245
random number: 684
writing to server: 785 245 684
message from server: 785 245 684
Connected...........
random number: 720
random number: 507
random number: 641
writing to server: 720 507 641
message from server: 720 507 641
```

