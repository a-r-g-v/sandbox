#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<arpa/inet.h>

int max(int a, int b) {
  if (a >= b) 
    return a;
  return b;
}

int main(int argc, char** argv) {

  int sockfd, code, len, listenfd;
  int maxfdp1, stdineof;
  struct sockaddr_in my_sock, peer_sock;
  char recvline[4096], sendline[4096];
  fd_set rset;

  bzero(&my_sock, sizeof(struct sockaddr_in));
  my_sock.sin_family = AF_INET;
  my_sock.sin_addr.s_addr = htonl(INADDR_ANY);
  my_sock.sin_port = htons(8888);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
    perror("failed to create socket");
    exit(1);
  }

  if ((code = connect(sockfd, (const struct sockaddr *)&my_sock, sizeof(struct sockaddr))) < 0 ) {
    perror("faild to connect");
    exit(1);
  }

  stdineof = 0;
  FD_ZERO(&rset);

  for(;;) {
    if  ( stdineof == 0 ){
      FD_SET(fileno(stdin), &rset);
    }
    FD_SET(sockfd, &rset);
    maxfdp1 = max(fileno(stdin), sockfd) + 1;

    if ( (code = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0 ) {
      perror("failed to select");
      exit(1);
    }

    if (FD_ISSET(fileno(stdin), &rset)) {
      if ( fgets(sendline, sizeof(sendline), stdin) == NULL ) {
          stdineof = 1;
          if ( shutdown(sockfd, SHUT_WR) < 0 ) {
            perror("failed to shutdown");
            exit(1);
          }
          FD_CLR(fileno(stdin), &rset);
          continue;
      }
        write(sockfd, sendline, strlen(sendline));
    }

    if (FD_ISSET(sockfd, &rset)) {
    int nread = 0;
    if ( (nread = read(sockfd, recvline, sizeof(recvline))) == 0 ) {
      if (stdineof == 1) {
        fprintf(stderr, "break stdineof == 1");
        break;
      }
      else {
        fprintf(stderr, "server terminated prematunerly\n");
        exit(1);
      }
    } 
      write(fileno(stdout), recvline, nread);
    }
  }
  close(sockfd);
}
