#include <stdio.h>
#include <stdlib.h>

void win() { system("/bin/sh"); }

void func() {
  char input[16];

  printf("%p\n",printf);
  printf("Please enter your name: \n");
  gets(input);

  printf("Hi %s\n", input);
}

int main() {
  func();
  return 0;
}
