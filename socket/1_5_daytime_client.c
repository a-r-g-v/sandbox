#include <stdlib.h>
#include <time.h>
#include <strings.h>
#include "lib/wrapsock.c"
#include <netinet/ip.h>
#include <arpa/inet.h>

int main(int argc, char **argv) 
{
  int sockfd, n;
  char recvline[MAXLINE+1];
  struct sockaddr_in servaddr;

  if (argc != 2) {
    fprintf(stderr, "usage: a.out <IPaddress>\n");
    exit(-1);
  }

  sockfd = Socket(AF_INET, SOCK_STREAM, 0);

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(13);

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
    fprintf(stderr, "inet_pton error for %s", argv[1]);
    exit(-1);
  }

  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
    perror("connect error");
  }

  int counter = 0;
  while( ( n = read(sockfd, recvline, MAXLINE)) > 0 ) {
    counter++;
    recvline[n] = 0; // null terminate
    if ( fputs(recvline, stdout) == EOF ) {
        perror("fputs error");
    }
  }

    if (n < 0 ) {
        perror("read error");
    }
    fprintf(stderr, "counter value is %d", counter);
    return 0;
}
