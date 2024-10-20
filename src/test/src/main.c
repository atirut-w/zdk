#include <stdio.h>

int main() {
  printf("Enter something: ");

  char buf[128] = {0};
  char *s = buf;
  int ch = 0;
  while (ch != '\r' || ch != -1) {
    ch = getchar();
    if (ch == '\r' || ch == -1) {
      putchar('\n');
      break;
    }
    *s++ = ch;
  }

  printf("You entered: %s\n", buf);

  return 0;
}
