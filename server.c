#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int sockFd, newSockFd;
void printErrorAndExit(const char *errorMsg) {
  perror(errorMsg);
  close(sockFd);
  close(newSockFd);
  exit(1);
}

int main() {

  sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockFd == -1)
    printErrorAndExit("Error in creating socket");
  else
    printf("Successfully socket created\n");

  struct sockaddr_in server;
  server.sin_port = 8889;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockFd, (struct sockaddr *)&server, sizeof(server)) == -1) {
    printErrorAndExit("Error in binding socket");
  } else {
    printf("Successfully binded\n");
  }

  if (listen(sockFd, 5) == -1) {
    printErrorAndExit("Error in listening for reueests");
  } else {
    printf("Listening for requests..\n");
  }

  struct sockaddr_in client;
  socklen_t clientAddrLen = sizeof(client);
  newSockFd = accept(sockFd, (struct sockaddr *)&client, &clientAddrLen);
  if (newSockFd == -1) {
    printErrorAndExit("Error in accepting connection");
  } else {
    printf("New connection accepted");
  }

  char msg[100];
  int count = 0;
  while (1) {
    bzero(msg, 100);
    if (recv(newSockFd, msg, sizeof(msg), 0) == -1) {
      printErrorAndExit("Error in receiving msg");
    }
    printf("\nClient: %s\n", msg);

    scanf("%s", msg);
    if (send(newSockFd, msg, sizeof(msg), 0) == -1) {
      printf("Error in sending message");
    }

    if (strncmp(msg, "exit", 4) == 0)
      break;
  }
  close(sockFd);
  close(newSockFd);
}