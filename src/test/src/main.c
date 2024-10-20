#include <stdio.h>

int main() {
  printf("Enter something: ");

  char buf[128];
  gets(buf);

  printf("You entered: %s\n", buf);

  return 0;
}
