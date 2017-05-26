#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 4096
#define LISTENQ 1024

int Socket(int family, int type, int protocol) 
{
  int n;
  if ( ( n = socket(family, type, protocol)) < 0 ) 
  {
    perror("failed to create socket");
  }
  return n;
}

void Bind(int sockfd, struct sockaddr *addr, socklen_t len)
{
  int n;
  if ( (  n = bind(sockfd, (const struct sockaddr *)addr, sizeof(struct sockaddr))) < 0 )
  {
    perror("faild to bind");
  }
}

void Listen(int sockfd, int backlog) 
{
  int n;
  if ( ( n = listen(sockfd, backlog)) < 0 ) 
  {
    perror("failed to listen");
  }
}

void Close(int fd) {
  int n;
  if ((n = close(fd)) == -1) {
    perror("close error");
  }
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
  int n;

again:

  if ( (n = accept(fd, sa, salenptr)) < 0) {
    if (errno == EPROTO || errno == ECONNABORTED) {
        goto again;
    } else {
        perror("failed to accept");
    }
  }
  return n;

}

int Write(int fd, const void *buf, size_t count) {
    ssize_t len;
    if ((len = write(fd, buf, count)) < 0 ) {
        perror("failed to write");
    }
    return len;
}
