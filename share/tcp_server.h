/*************************************************************************
	> File Name: tcp_server.h
	> Author: 
	> Mail: 
	> Created Time: 六  9/10 13:11:13 2016
 ************************************************************************/

#ifndef NETWORK_SHARE_TCP_SERVER_H
#define NETWORK_SHARE_TCP_SERVER_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#define MAXLINE 256

void err_sys(const char *fmt, ...);
void err_doit(int errnoflag, int error, const char *fmt, va_list ap);
int Socket(int domain, int type, int protocol);
int Bind(int socket, const struct sockaddr *address, socklen_t address_len);
int Listen(int socket, int backlog);
int Accept(int socket, struct sockaddr *address, socklen_t *address_len);
int Connect(int socket, const struct sockaddr *address,
           socklen_t address_len);
char *Fgets(char *ptr, int n, FILE *stream);
void client_info(int connfd);
int tcp_echo(int remote_fd);
ssize_t Recv(int socket, void *buffer, size_t length, int flags);
void set_address(struct sockaddr_in *server,const char *serverip, const char *port);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
void cli_str(FILE *fp, int sockfd);
size_t Read(int fd, void *buf, size_t cout);
size_t Write(int fd, void *buf, size_t cout);
int Shutdown(int sockfd, int how);

#endif /* NETWORK_SHARE_TCP_SERVER_H */ 
