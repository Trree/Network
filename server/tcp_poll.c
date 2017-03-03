/*************************************************************************
	> File Name: tcp_server.c
	> Author: 
	> Mail: 
	> Created Time: 二  9/ 6 21:28:07 2016
 ************************************************************************/

#include "../share/tcp_server.h"
#include <limits.h>
#define INFTIM -1
#define OPEN_MAX 1024

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
    Listen(listenfd, LISTENQ);

    struct pollfd client[OPEN_MAX];
    int i, n,maxi, sockfd, nready;
    char buf[MAXLINE];
    for (i = 0; i < OPEN_MAX; i++) {
        client[i].fd = -1;
    }
    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    maxi = 0;

    socklen_t remote_len = sizeof(struct sockaddr_in);
    for ( ; ; ) {
        printf("Start poll .....\n");
        nready = poll(client, maxi + 1, INFTIM);
        if (client[0].revents & POLLRDNORM) {
            printf("Listening Socket is readable\n");
            remote_len = sizeof(clientaddr);
            connfd = accept(listenfd, (struct sockaddr *)&serveraddr, &remote_len);
            printf("New incoming connection - %d\n", connfd);

            for (i = 0; i < OPEN_MAX; i++) {
                if (client[i].fd < 0) {
                    client[i].fd = connfd;
                    break;
                }
                if (i == OPEN_MAX) {
                    err_sys("too many client");
                }
                client[i].events = POLLRDNORM;
                if (i > maxi) {
                    maxi = i;
                }
                if (--nready <= 0) {
                    continue;
                }
            }

            for (i = 1; i <= maxi; i++) {
                if ((sockfd = client[i].fd < 0)) {
                    continue;
                }
                if (client[i].revents & (POLLRDNORM | POLLERR)) {
                    printf(" Descriptor %d is readable\n", i);
                    if ((n = read(sockfd, buf, MAXLINE)) < 0) {
                        if (errno == ECONNRESET) {
                            close(sockfd);
                            client[i].fd = -1;
                        }
                        else {
                            err_sys("read error");
                        }
                    }
                    else if (n == 0) {
                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else {
                        Write(sockfd, buf, n);
                    }

                    if (--nready <= 0) {
                        break;
                    }
                }
            }
        }
    }
    return 0;
}
