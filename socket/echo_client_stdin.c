#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<arpa/inet.h>

int main(int argc, char** argv) {

  int sockfd, code, len, listenfd;
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

  if ((code = connect(sockfd, (const struct sockaddr *)&my_sock, sizeof(struct sockaddr))) < 0 ) {
    perror("faild to connect");
    exit(1);
  }

  for(;;) {
    if ( fgets(buff, sizeof(buff), stdin) != NULL ) {
      write(sockfd, buff, strlen(buff));
    }

    int nread = 0;
    if ( (nread = read(sockfd, buff, sizeof(buff))) > 0 ) {
      fputs(buff, stdout);
    } 
  }
  close(sockfd);
}
