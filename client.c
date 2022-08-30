#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int sockFd;
void printErrorAndExit(const char *errorMsg) {
  perror(errorMsg);
  close(sockFd);
  exit(1);
}

int main() {
  struct sockaddr_in server;
  int sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockFd == -1) {
    printErrorAndExit("Error in creating socket");
  } else {
    printf("Socket created successfully\n");
  }

  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = 8889;
  server.sin_family = AF_INET;

  if (connect(sockFd, (const struct sockaddr *)&server, sizeof(server)) == -1) {
    printErrorAndExit("Error in connecting");
  } else {
    printf("Successfully connected\n");
  }

  char msg[100];
  int count = 5;
  while (1) {
    bzero(msg, 100);
    scanf("%s", msg);
    if (send(sockFd, msg, sizeof(msg), 0) == -1) {
      printErrorAndExit("Error in sending..");
    }
    bzero(msg, 100);
    if (recv(sockFd, msg, sizeof(msg), 0) == -1) {
      printErrorAndExit("Error in receiving..");
    }
    printf("\nServer: %s\n", msg);
    if (strncmp(msg, "exit", 4) == 0)
      break;
  }
  close(sockFd);
}