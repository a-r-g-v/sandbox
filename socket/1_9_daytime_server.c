#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <strings.h>
#include "lib/wrapsock.c"
#include <netinet/ip.h>

// Page: 13

int main(int argc, char** argv) 
{
  int listenfd, connfd;
  struct sockaddr_in serveraddr, cliaddr;
  char buff[MAXLINE];
  time_t ticks;
  
  listenfd  = Socket(AF_INET, SOCK_STREAM, 0);
  bzero(&serveraddr, sizeof(serveraddr));

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(13);

  Bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
  Listen(listenfd, LISTENQ);

  for (;; ) {
    int len = sizeof(cliaddr);
    connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &len);
    if( ( (void *)inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff))) == NULL) {
      perror("failed to inet_ntop");
    }
    printf("connection from: %s port %d \n", buff, ntohs(cliaddr.sin_port));
    ticks = time(NULL);
    snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
    int i;
    for (i = 0; i < strlen(buff); i++) {
      Write(connfd, &buff[i], 1);
    }
    Close(connfd);
  }


}
