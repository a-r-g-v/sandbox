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

  for (;;) {


    len = sizeof(struct sockaddr_in);

    if (( listenfd = accept(sockfd, (struct sockaddr *)&peer_sock, &len) ) < 0 ) {
      if (errno == EINTR)
        continue;
      perror("failed to accept");
      exit(1);
    }
    
    pid_t child;
    if ((child = fork()) == 0) {
    close(sockfd);
    fprintf(stderr, "fetch connection by backlog queue : %s:%d \n", 
        inet_ntoa(peer_sock.sin_addr), peer_sock.sin_port);
    size_t nread;
    while (( nread = read(listenfd, buff, sizeof(buff))) > 0) {
        write(listenfd, buff, nread);
        fprintf(stderr, "write: %d bytes\n", nread);
    }
    close(listenfd); 
    fprintf(stderr, "close connection\n");
    exit(0);
    }
 }


  return 0;
}
