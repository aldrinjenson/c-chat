#include "headers/utils.h"
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// #include <curses.h>

#define MAX_ALLOWED_CONNECTIONS 3
#define MAX_USERNAME_LENGTH 15
#define MAX_MESSAGE_LENGTH 1024
#define MAX_BACKLOG_CONNECTIONS 10

int thisServerSockFd;
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

// void printErrorAndExit(const char *errorMsg) {
//   perror(errorMsg);
//   close(thisServerSockFd);
//   for (int i = 0; i < connectedSocketsCount; i++) {
//     close(connectedSockets[i].socketFd);
//   }
//   exit(1);
// }
void sendMessageToAllConnectedClients(char *from, char *message,
                                      int fromSocketId) {

  strcpy(msgObj.from, from);
  strcpy(msgObj.message, message);

  printf("\nmsg from %s: '%s'\n", from, message);
  for (int i = 0; i < connectedSocketsCount; i++) {
    if (connectedSockets[i].socketFd == fromSocketId ||
        connectedSockets[i].socketFd == thisServerSockFd)
      continue;
    if (send(connectedSockets[i].socketFd, (char *)&msgObj, sizeof(msgObj),
             0) <= 0) {
      printf("Error in sending message");
    }
  }
}

void *acceptNewSocketWhenNeeded() {
  if (listen(thisServerSockFd, MAX_BACKLOG_CONNECTIONS) == -1) {
    printErrorAndExit("Error in listening for reqeests");
  } else {
    printf("Listening for requests..\n");
  }

  struct sockaddr_in client;
  socklen_t clientAddrLen = sizeof(client);
  int newSockFd =
      accept(thisServerSockFd, (struct sockaddr *)&client, &clientAddrLen);
  if (newSockFd == -1) {
    printErrorAndExit("Error in accepting new connection");
  } else {
    printf("New connection accepted\n");
  }
  struct socketObj newSocketConn;
  char username[MAX_USERNAME_LENGTH];
  if (recv(newSockFd, username, sizeof(username), 0) == -1) {
    printErrorAndExit("Error in receiving username");
  }
  check(send(newSockFd, (char *)&connectedSocketsCount,
             sizeof(connectedSocketsCount), 0),
        "Error in sending user count to client");
  newSocketConn.socketFd = newSockFd;
  strcpy(newSocketConn.username, username);
  connectedSockets[connectedSocketsCount++] = newSocketConn;

  // sending the number of users present in room to client.

  char newClientJoinedMessage[40];
  strcat(newClientJoinedMessage, username);
  strcat(newClientJoinedMessage, " has joined the chat room");
  sendMessageToAllConnectedClients("ServerAdmin", newClientJoinedMessage,
                                   newSockFd);

  char msg[MAX_MESSAGE_LENGTH];
  while (1) {
    bzero(msg, MAX_MESSAGE_LENGTH);
    if (recv(newSockFd, msg, sizeof(msg), 0) <= 0) {
      printErrorAndExit("Error in receiving msg");
    }
    sendMessageToAllConnectedClients(username, msg, newSockFd);
  }
  return NULL;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printErrorAndExit("Enter port number as second argument!");
  }

  thisServerSockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (thisServerSockFd == -1)
    printErrorAndExit("Error in creating socket");
  else
    printf("Successfully socket created\n");

  struct sockaddr_in server;
  server.sin_port = atoi(argv[1]);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(thisServerSockFd, (struct sockaddr *)&server, sizeof(server)) ==
      -1) {
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