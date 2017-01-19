/*************************************************************************
	> File Name: tcp_server.c
	> Author: 
	> Mail: 
	> Created Time: 二  9/ 6 21:28:07 2016
 ************************************************************************/

#include "../share/tcp_server.h"

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

    int maxfd;
    int sockfd;
    int nready;
    int client[FD_SETSIZE];
    int i;
    int maxi;
    char buf[MAXLINE];
    ssize_t n;
    maxfd = listenfd;
    fd_set rset, allset;
    for (i = 0; i < FD_SETSIZE; i++) {
        client[i] = -1;
    } 
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    socklen_t remote_len = sizeof(struct sockaddr_in);
    for ( ; ; ) {
        rset = allset;
        nready = Select(maxfd+1, &rset, NULL, NULL, NULL);
        if (FD_ISSET(listenfd, &rset)) {
            connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &remote_len);

            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
                if (i == FD_SETSIZE) {
                    err_sys("too many clients");
                }
                FD_SET(connfd, &allset);
                if(connfd > maxfd) {
                    maxfd = connfd;
                }
                if (i > maxi) {
                    maxi = i;
                }
                if (--nready <= 0) {
                    continue;
                }
            }

            for (i = 0; i <= maxi; i++) {
                if ((sockfd = client[i]) < 0) {
                    continue;
                }
                if (FD_ISSET(sockfd, &rset)) {
                    if ((n = Read(sockfd, buf, MAXLINE)) == 0) {
                        close(sockfd);
                        FD_CLR(sockfd, &rset);
                        client[i] = -1;
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
