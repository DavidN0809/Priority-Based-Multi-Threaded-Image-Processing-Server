#include "csapp.h"

int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Rio_readinitb(&rio, connfd);

        size_t n;
        char buf[MAXLINE];
        while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
            printf("server received %d bytes\n", (int)n);
            Rio_writen(connfd, buf, n);
        }
        Close(connfd);
    }
}
