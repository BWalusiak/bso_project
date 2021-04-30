#include <stdio.h>
#include <stdlib.h>

void func() {
  char input[16];
  char leak[32];


  // canary leak
  gets(leak);
  printf(leak);
  printf("\n");

  printf("Please enter your name: \n");
  gets(input);

  printf("Hi %s\n", input);
}

int main() {
  func();
  return 0;
}
