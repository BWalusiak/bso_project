#include <stdio.h>
#include <stdlib.h>

void func() {
  char input[512];
  char *buff = alloca(10);

  printf("Please enter your name: \n");
  fgets(input, 512, stdin);
  fgets(buff, 10, stdin);
  printf("Hi %s\n", input);
  printf("%s\n", buff);
}

int main() {
  func();
  return 0;
}
