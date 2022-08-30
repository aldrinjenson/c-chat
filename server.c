#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_ALLOWED_CONNECTIONS 3
#define MAX_USERNAME_LENGTH 15
#define MAX_MESSAGE_LENGTH 1024

int sockFd;
int errorFound = 0;
struct socketObj {
  int socketFd;
  char username[MAX_USERNAME_LENGTH];
};

struct messageObj {
  char message[MAX_MESSAGE_LENGTH];
  char from[MAX_MESSAGE_LENGTH];
} msgObj;

struct socketObj connectedSockets[MAX_ALLOWED_CONNECTIONS];
int connectedSocketsCount = 0;

void printErrorAndExit(const char *errorMsg) {
  errorFound = 1;
  perror(errorMsg);
  close(sockFd);
  for (int i = 0; i < connectedSocketsCount; i++) {
    close(connectedSockets[i].socketFd);
  }
  exit(1);
}

void *acceptNewSocketWhenNeeded() {
  if (listen(sockFd, 5) == -1) {
    printErrorAndExit("Error in listening for reqeests");
  } else {
    printf("Listening for requests..\n");
  }

  struct sockaddr_in client1;
  socklen_t clientAddrLen = sizeof(client1);
  int newSockFd1 = accept(sockFd, (struct sockaddr *)&client1, &clientAddrLen);
  if (newSockFd1 == -1) {
    printErrorAndExit("Error in accepting new connection");
  } else {
    printf("New connection accepted\n");
  }
  struct socketObj newSocketConn;
  char username[MAX_USERNAME_LENGTH];
  if (recv(newSockFd1, username, sizeof(username), 0) == -1) {
    printErrorAndExit("Error in receiving username");
  }
  printf("%s has joined the chat", username);
  newSocketConn.socketFd = newSockFd1;
  strcpy(newSocketConn.username, username);
  connectedSockets[connectedSocketsCount++] = newSocketConn;

  char msg[MAX_MESSAGE_LENGTH];
  while (!errorFound) {
    bzero(msg, MAX_MESSAGE_LENGTH);
    if (recv(newSockFd1, msg, sizeof(msg), 0) == -1) {
      printErrorAndExit("Error in receiving msg");
    }

    strcpy(msgObj.from, username);
    strcpy(msgObj.message, msg);

    printf("\n%s: %s\n", username, msg);
    for (int i = 0; i < connectedSocketsCount; i++) {
      if (connectedSockets[i].socketFd == newSockFd1)
        continue;
      if (send(connectedSockets[i].socketFd, (char *)&msgObj, sizeof(msgObj),
               0) <= 0) {
        printf("Error in sending message");
      } else {
        printf("Message sent to client");
      }
    }
  }
  return NULL;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printErrorAndExit("Enter port number as second argument!");
  }

  sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockFd == -1)
    printErrorAndExit("Error in creating socket");
  else
    printf("Successfully socket created\n");

  struct sockaddr_in server;
  server.sin_port = atoi(argv[1]);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockFd, (struct sockaddr *)&server, sizeof(server)) == -1) {
    printErrorAndExit("Error in binding socket");
  } else {
    printf("Successfully binded\n");
  }

  pthread_t threads[MAX_ALLOWED_CONNECTIONS];
  for (int i = 0; i < MAX_ALLOWED_CONNECTIONS; i++) {
    pthread_create(&threads[i], NULL, acceptNewSocketWhenNeeded, NULL);
  }
  pthread_exit(NULL);
  for (int i = 0; i < connectedSocketsCount; i++) {
    close(connectedSockets[i].socketFd);
  }

  return 0;
}