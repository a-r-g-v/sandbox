#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include <signal.h>




void sig_chld(int signo) 
{
  pid_t pid;
  int stat;
  while ( (pid = waitpid(-1, &stat, WNOHANG)) >0 )  {
    fprintf(stderr, "child %d terminated\n", pid);
  }
  return;
}


int main(int argc, char** argv) {

  int sockfd, code, len, listenfd;
  int maxi, i;
  int nready, client[FD_SETSIZE];
  ssize_t n;
  fd_set rset, allset;
  struct sockaddr_in my_sock, peer_sock;
  char buff[4096];

  bzero(&my_sock, sizeof(struct sockaddr_in));
  my_sock.sin_family = AF_INET;
  my_sock.sin_addr.s_addr = htonl(INADDR_ANY);
  my_sock.sin_port = htons(8888);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
    perror("failed to create socket");
    exit(1);
  }

  if ((code = bind(sockfd, (struct sockaddr *)&my_sock, sizeof(struct sockaddr))) < 0){
    perror("failed to bind socket");
    exit(1);
  }
  if ((code = listen(sockfd, 4096)) < 0 ) {
    perror("failed to listen");
  }
  signal(SIGCHLD, sig_chld);

  int maxfd = sockfd;
  maxi -= 1;
  for (i = 0; i < FD_SETSIZE; i++) {
    client[i] = -1;
  }
  FD_ZERO(&allset);
  FD_SET(sockfd, &allset);

  for (;;) {
    rset = allset;

    nready = select(maxfd+1, &rset, NULL, NULL, NULL);

    if (FD_ISSET(sockfd, &rset)) {
      len = sizeof(struct sockaddr_in);
      if (( listenfd = accept(sockfd, (struct sockaddr *)&peer_sock, &len) ) < 0 ) {
        if (errno == EINTR)
          continue;
        perror("failed to accept");
        exit(1);
      }
        fprintf(stderr, "fetch connection by backlog queue : %s:%d \n", 
            inet_ntoa(peer_sock.sin_addr), peer_sock.sin_port);

      for (i = 0; i <FD_SETSIZE; i++) {
        if (client[i] < 0 ) {
          client[i] = listenfd;
          break;
        }
      }
      if(i == FD_SETSIZE) {
        fprintf(stderr, "too many clients\n");
        exit(1);
      }

      FD_SET(listenfd, &allset);

      if (listenfd > maxfd) {
        maxfd = listenfd;
      }
      if (i > maxi) {
        maxi = i;
      }

      if (-nready <= 0) {
        continue;
      }

    }
    for (i = 0; i <= maxi; i++) {
      if ( (listenfd = client[i]) < 0 ) {
        continue;
      }
      if (FD_ISSET(listenfd, &rset)) {
        size_t nread;

        if (( nread = read(listenfd, buff, sizeof(buff))) == 0) {
          close(listenfd); 
          FD_CLR(listenfd, &allset);
          client[i] = -1;
          fprintf(stderr, "close connection\n");
        } else {
          write(listenfd, buff, nread);
          fprintf(stderr, "write: %d bytes\n", nread);
        }

        if (-nread <= 0 ) {
          break;
        }
      }

    }

  }


  return 0;
}
