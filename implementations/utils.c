#include "../headers/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printErrorAndExit(const char *errorMsg) {
  perror(errorMsg);
  exit(1);
}

void check(int val, char *errorMsg) {
  if (val < 0) {
    printErrorAndExit(errorMsg);
  }
}
