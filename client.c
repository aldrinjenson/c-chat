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

#define MAX_MESSAGE_LENGTH 1024
#define MAX_USERNAME_LENGTH 15

int sockFd;
// void printErrorAndExit(const char *errorMsg) {
// perror(errorMsg);
//   close(sockFd);
//   exit(1);
// }

struct messageObj {
  char message[MAX_MESSAGE_LENGTH];
  char from[MAX_MESSAGE_LENGTH];
};

void *receiveMessagesFromServer() {
  struct messageObj msgObj;
  while (1) {
    if (recv(sockFd, (char *)&msgObj, sizeof(msgObj), 0) <= -0) {
      printErrorAndExit("Error in receiving...");
    } else {
      if (strlen(msgObj.message))
        printf("\n%s: %s\n", msgObj.from, msgObj.message);
    }
  }
  return NULL;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printErrorAndExit("Enter port number as second argument!");
  }

  struct sockaddr_in server;
  sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockFd == -1) {
    printErrorAndExit("Error in creating socket");
  } else {
    printf("Socket created successfully\n");
  }

  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = atoi(argv[1]);
  server.sin_family = AF_INET;

  if (connect(sockFd, (const struct sockaddr *)&server, sizeof(server)) == -1) {
    printErrorAndExit("Error in connecting");
  } else {
    printf("Successfully connected\n");
  }

  pthread_t thread1;
  pthread_create(&thread1, NULL, receiveMessagesFromServer, NULL);

  char username[MAX_USERNAME_LENGTH];
getUserName:
  printf("Enter username: ");
  scanf("%s", username);
  if (strlen(username) < 3) {
    printf("Please enter a name greater than 2 characters\n");
    goto getUserName;
  }
  username[strlen(username)] = '\0';

  if (send(sockFd, username, sizeof(username), 0) == -1) {
    printErrorAndExit("Error in sending..");
  }
  int userCount;
  check(recv(sockFd, (char *)&userCount, sizeof(userCount), 0),
        "Error in receiving...");
  printf("\n%d other user(s) in the room right now\n", userCount);
  printf("\nWelcome to C-Chat!\nEnter a message to broadcast it to everyone "
         "else in room.");

  char msg[MAX_MESSAGE_LENGTH];
  while (1) {
    bzero(msg, MAX_MESSAGE_LENGTH);
    fgets(msg, MAX_MESSAGE_LENGTH, stdin);
    if (strlen(msg) > 1 && send(sockFd, msg, sizeof(msg), 0) <= 0) {
      printErrorAndExit("Error in sending..");
    }
    if (strncmp(msg, "exit", 4) == 0)
      break;
  }
  pthread_exit(NULL);
  close(sockFd);
}