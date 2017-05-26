#include<stdlib.h>
#include<stdio.h>
#include<strings.h>
#include<string.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<arpa/inet.h>


#define NUM_WORKER 100

int main(int argc, char** argv) {

  int sockfd[NUM_WORKER];
  struct sockaddr_in my_sock;
  char buff[NUM_WORKER][4096];

  if (argc != 2) {
    fprintf(stderr, "%s <data>\n", argv[0]);
    exit(1);
  }

  bzero(&my_sock, sizeof(struct sockaddr_in));
  my_sock.sin_family = AF_INET;
  my_sock.sin_addr.s_addr = htonl(INADDR_ANY);
  my_sock.sin_port = htons(8888);

  int i;
  for (i = 0; i < NUM_WORKER; i++ ){
  if ((sockfd[i] = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
      perror("failed to create socket");
      exit(1);
  }
  int code;
  if ((code = connect(sockfd[i], (const struct sockaddr *)&my_sock, sizeof(struct sockaddr))) < 0 ) {
    perror("faild to connect");
    exit(1);
  }

  write(sockfd[i], argv[1], strlen(argv[1]));
  read(sockfd[i], buff[i], sizeof(buff[i]));
  fprintf(stderr, "%s\n", buff[i]);
  }


  for (i = 0; i < NUM_WORKER; i++ ){
  close(sockfd[i]);
  }
}
