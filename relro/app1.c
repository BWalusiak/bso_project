#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void func() {
  char input[200];

  printf("Please enter your name: \n");
  gets(input);
  printf(input);

  scanf("%s", input);
  puts(input);
}

int main() {
  func();
  return 0;
}
