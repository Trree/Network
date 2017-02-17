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
    int on = 1;
    int rc;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    rc = setsockopt(listenfd, SOL_SOCKET,  SO_REUSEADDR,(char *)&on, sizeof(on));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(listenfd);
        exit(-1);
    }
    rc = ioctl(listenfd, FIONBIO, (char *)&on);
    if (rc < 0) {
        printf("ioctl() failed");
        close(listenfd);
        exit(-1);
    }
    set_address(&serveraddr, serverip, serverport);
    Bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); 
    Listen(listenfd, LISTENQ);

    int maxfd;
    int nready;
    int desc_ready;
    int i;
    int close_conn;
    char buf[MAXLINE];
    ssize_t n;
    struct timeval tv;
    maxfd = listenfd;
    fd_set read_fds, master;
    FD_ZERO(&master);
    FD_SET(listenfd, &master);

    socklen_t remote_len = sizeof(struct sockaddr_in);
    for ( ; ; ) {
        tv.tv_sec = 20;  
        tv.tv_usec = 0;
        memcpy(&read_fds, &master, sizeof(master));
        printf("Waiting on select()...\n");
        nready = Select(maxfd+1, &read_fds, NULL, NULL, &tv);
        desc_ready = nready;
        for(i = 0; i <= maxfd && desc_ready > 0; i++) {
            if (FD_ISSET(i, &read_fds)) {
                desc_ready -= 1;
                if (i == listenfd) {
                    printf("Listening socket is readable\n");
                    do {
                        connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &remote_len);
                        printf("  New incoming connection - %d\n", connfd);
                        FD_SET(connfd, &master);
                        if(connfd > maxfd) {
                            maxfd = connfd;
                        }
                    } while(connfd != -1);
                }
                else {
                    printf("  Descriptor %d is readable\n", i);
                    close_conn = FALSE;
                        
                    n = recv(i, buf, MAXLINE, 0);
                    if (n < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("  recv() failed");
                            close_conn = TRUE;
                        }
                    }
                    if (n == 0) {
                        printf("  Connection closed\n");
                        close_conn = TRUE;
                    }

                    n = write(i, buf, n);
                    if (n < 0 ) {
                        perror("  send() failed");
                        close_conn = TRUE;
                    }

                    if (close_conn) {
                        close(i);
                        FD_CLR(i, &master);
                        if (i == maxfd) {
                            while (FD_ISSET(maxfd, &master) == FALSE)
                                maxfd -= 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}
