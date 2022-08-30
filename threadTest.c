#include <pthread.h>
#include <stdio.h>

struct threadInfo {
  char *threadId;
  char symbol;
};

void *func(void *arg) {
  char symb = (char)arg;
  for (int i = 0; i < 1000; i++) {
    printf("%c", symb);
  }
  return NULL;
}

int main() {
  char *symbols = "abcdefghijklm";
  pthread_t listeners[10];
  for (int i = 0; i < 10; i++) {
    pthread_create(&listeners[i], NULL, func, symbols[i]);
  }

  pthread_exit(NULL);
}