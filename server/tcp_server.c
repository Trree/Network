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
    pid_t childpid;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    set_address(&serveraddr, serverip, serverport);
    Bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)); 
    Listen(listenfd, 5);
   
    socklen_t remote_len = sizeof(struct sockaddr_in);
    for ( ; ; ) {
        connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &remote_len);
        if ((childpid = fork()) == 0) {
            close(listenfd);
            client_info(connfd);
            tcp_echo(connfd);
            exit(0);
        }
        close(connfd);
    }

    return 0;
}
