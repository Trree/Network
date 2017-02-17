#include "tcp_server.h"
#include <sys/select.h>
#include <sys/time.h>

#define max(a, b) \
        __extension__ ({ __typeof__ (a) _a = (a);\
          __typeof__ (b) _b = (b);\
          _a > _b ? _a : _b; })

void cli_str(FILE *fp, int sockfd)
{
    int maxfd, stdineof;
    fd_set rset;
    int n;
    char buffer[MAXLINE];

    stdineof = 0;
    FD_ZERO(&rset);
    for(; ;) {
        if (stdineof == 0) {
            FD_SET(fileno(fp), &rset);
        }
        FD_SET(sockfd, &rset);
        maxfd = max(fileno(fp), sockfd) + 1;
        Select(maxfd, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            if ((n = Read(sockfd, buffer, MAXLINE)) == 0) {
                if (stdineof == 1) {
                    return;
                }
                else {
                    err_sys("str_cli: server terminated prematurely");
                }
            }
            Write(fileno(stdout), buffer, n);
        }

        if (FD_ISSET(fileno(fp), &rset)) {
            if ((n = Read(fileno(fp), buffer, MAXLINE)) == 0) {
                stdineof = 1;
                Shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            Write(sockfd, buffer, n);
        }
    }
}
