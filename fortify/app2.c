#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  struct S {
    struct T {
      char buf[5];
      int x;
    } t;
    char buf[20];
  } var;
  strcpy(&var.t.buf[1], "abcdefg");
  printf("%s\n", var.t.buf);
  return 0;
}
