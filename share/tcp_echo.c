#include "tcp_server.h"

int tcp_echo(int remote_fd)
{
    int revn;
    int sendn;
    char buff[MAXLINE];
    //revn = Recv(remote_fd, buff, sizeof(buff), 0);
    revn = recv(remote_fd, buff, sizeof(buff), 0);
    printf("%s", buff);
    sendn = send(remote_fd, buff, revn, 0);
    
    return sendn;
}

void client_info(int connfd)
{
    socklen_t len;
    struct sockaddr_storage addr;
    char ipstr[INET6_ADDRSTRLEN];
    int port;

    len = sizeof(addr);
    getpeername(connfd, (struct sockaddr *)&addr, &len);

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
    }
    else {
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        port = ntohs(s->sin6_port);
        inet_ntop(AF_INET, &s->sin6_addr, ipstr, sizeof(ipstr));
    }

    printf("Peer IP address: %s\n", ipstr);
    printf("Peer port      :%d\n", port);
}

