/*************************************************************************
	> File Name: tcp_client.c
	> Author: 
	> Mail: 
	> Created Time: 六  9/17 17:21:07 2016
 ************************************************************************/

#include "../share/tcp_server.h"

int Send(int socket, const void *buf, int buflen, int flag)
{
    int sendn;
    sendn= send(socket, buf, buflen, flag);
    if (sendn < 0) {
        err_sys("send error.");
    }

    return sendn;
}

int Recverr(int socket, void *buf, int length, int flag)
{
    int revn;
    if ((revn = recv(socket, buf, length, flag)) < 0){
        err_sys("Recverr error");
    }

    return revn;
}

void client_echo(FILE *fp, int connfd)
{
    char buf[MAXLINE];
    char rev[MAXLINE];
    int buflen;
    buflen = sizeof(buf);
    bzero(buf,buflen);
    while ((Fgets(buf, buflen, fp)) != NULL) {
        Send(connfd, buf, buflen, 0);
        Recverr(connfd, rev, MAXLINE, 0);
        fputs(rev, stdout);
    }
}

int main(int argc, char *argv[]) 
{
    const char *serverip = NULL;
    const char *serverport = NULL;
    if (argc < 3) {
        printf("Usage: server serverip serverport");
        exit(0);
    }
    else {
        serverip = argv[1];
        serverport = argv[2];
    }
    
    int connfd;
    struct sockaddr_in serveraddr;
    set_address(&serveraddr, serverip, serverport);
    connfd = Socket(AF_INET, SOCK_STREAM, 0);
    Connect(connfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
    client_echo(stdin, connfd);
    cli_str(stdin, connfd);
    close(connfd);

    return 0;
}

