/*************************************************************************
	> File Name: set_socket.c
	> Author: 
	> Mail: 
	> Created Time: 六  9/10 13:12:22 2016
 ************************************************************************/
#include "tcp_server.h"

/*
 * 在端口转换的时候需要检查端口是不是正确的，用strtol转换成一个合法的数值
 * 使用服务器地址的时候需要检查是IP输入的还是域名输入
 * 使用域名的时候需要使用 gethostbyname 函数。
 */
void set_address(struct sockaddr_in *server,const char *serverip, const char *port)
{
    int serverport;
    struct hostent *hp;
    char *endptr;
    struct servent *sp;

    bzero(server, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;

    serverport = strtol(port, &endptr, 0);
    if (*endptr == '\0') {
        server->sin_port = htons(serverport);
    }
    else {
        sp = getservbyname(port, "tcp");
        if (NULL == sp) {
            printf("unkown service: %s\n", port);
        }
        server->sin_port = sp->s_port;
    }
    if (serverip != NULL) {
        if(!inet_pton(AF_INET, serverip, &(server->sin_addr))) {
            hp = gethostbyname(serverip);
            if (NULL == (struct hostent *)hp) {
                printf("unkown host: %s\n", serverip);
            }
            server->sin_addr = *((struct in_addr *)hp->h_addr);
        }
    }
    else {
        server->sin_addr.s_addr = INADDR_ANY;
    }
}

void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
    char buf[MAXLINE];
    vsnprintf(buf, MAXLINE, fmt, ap);
    if (errnoflag) {
        snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ":%s", strerror(error));
    }
    strcat(buf, "\n");
    fflush(stdout);
    fputs(buf, stderr);
    fflush(NULL);
}

void err_sys(const char *fmt, ...) 
{
    va_list ap;

    va_start(ap, fmt);
    err_doit(1, errno, fmt, ap);
    va_end(ap);
    exit(1);
}

/*
void err_quit(char *fmt, ...) 
{
    int res;
    va_list list;
    va_start(list, fmt);
    res = vfprintf(stderr, fmt, list);
    if (errno != 0) {
        fprintf(stderr, " : %s", strerror(errno));
    }
    fprintf(stderr, "\n", list);
    va_end(list);
    exit(1);
}
*/

char *Fgets(char *ptr, int n, FILE *stream)
{
    char *rptr = fgets(ptr, n, stream);
    if (rptr == NULL && ferror(stream)) {
        err_sys("fgets error");
    }
    return rptr;
}


int Socket(int domain, int type, int protocol)
{
    int n;
    n = socket(domain, type, protocol);
    if (n < 0) {
        err_sys("Socket error");
    }
    return n;
}

int Bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
    int n;
    if ((n = bind(socket, address, address_len)) < 0) {
        err_sys("Bind error");
    }

    return n;
}


int Listen(int socket, int backlog) 
{
    int n;
    if ((n = listen(socket, backlog)) < 0) {
        err_sys("Listen error");
    }
    return n;
}

int Accept(int socket, struct sockaddr *address, socklen_t *address_len)
{
    int n;
    if ((n = accept(socket, address, address_len)) < 0) {
        err_sys("Accept error");
    }

    return n;
}

int Connect(int socket, const struct sockaddr *address,
           socklen_t address_len)
{
    int n;
    if ((n = connect(socket, address, address_len)) < 0) {
        err_sys("Connect error.");
    }

    return n;
}

/*
 * 实现对输入的字符串完全输入
 */
ssize_t Recv(int socket, void *buffer, size_t length, int flags)
{
    int nread;
    char *bufp = buffer;
    int nleft = length;
    while(nleft > 0) {
        nread = recv(socket, bufp, nleft, flags);
        if (nread < 0) {
            /*注意这个错误的产生是由于什么原因*/
            if (errno == EINTR) {
                nread = 0;
            }
            else {
                return -1;
            }
        }
        /*这个说明了什么？EOF*/
        if (nread == 0) {
            break;
        }

        bufp += nread;
        nleft -= nread;
    }

    return (length - nleft);
}

int tcp_echo(int remote_fd)
{
    int revn;
    int sendn;
    char buff[MAXLINE];
    revn = Recv(remote_fd, buff, sizeof(buff), 0);
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

