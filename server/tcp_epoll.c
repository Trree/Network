/*************************************************************************
	> File Name: tcp_server.c
	> Author: 
	> Mail: 
	> Created Time: 二  9/ 6 21:28:07 2016
 ************************************************************************/

#include "../share/tcp_server.h"
#define MAXEVENTS 64
#include <sys/epoll.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    const char *serverip = NULL;
    const char *serverport = NULL;
    if (argc < 2) {
        printf("Usage: port or ip port");
        exit(0);
    }
    else if (argc == 2) {
        serverport = argv[1];
    }
    else{
        serverip = argv[1];
        serverport = argv[2];
    }
    
    
    int listenfd;
    int connfd;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    set_address(&serveraddr, serverip, serverport);
    Bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); 
    int ret = make_socket_non_blocking(listenfd);
    Listen(listenfd, 5);

    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    efd = epoll_create1(0);
    if (efd == -1) {
        perror("epoll_create");
        abort();
    }

    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &event);
    if (ret == -1) {
        perror("epoll_ctl");
        abort();
    }

    events = calloc(MAXEVENTS, sizeof(event));
   
    socklen_t remote_len = sizeof(struct sockaddr_in);
    while(1) {
        int n, i;
        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for(i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN))) {
                    fprintf(stderr, "epoll error\n");
                    close(events[i].data.fd);
                    continue;
            }
            else if (listenfd == events[i].data.fd) {
                while(1) {
                    connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &remote_len);
                    if (connfd == -1) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            break;
                        }
                        else {
                            perror("accept");
                            break;
                        }
                    }
                    client_info(connfd);
                    ret = make_socket_non_blocking(connfd);
                    if (ret == -1) {
                        abort();
                    }
                    event.data.fd = connfd;
                    event.events = EPOLLIN | EPOLLET;
                    ret = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event);
                    if (ret == -1) {
                        perror("epoll_ctl");
                        abort();
                    }
                }
                continue;
            }
            else {
                tcp_echo(events[i].data.fd);
            }

        }
    }
    
    free(events);
    close(connfd);

    return 0;
}
