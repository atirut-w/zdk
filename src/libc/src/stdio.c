#include <stdarg.h>
#include <stdio.h>

#ifndef EOL
#define EOL '\r'
#endif
#ifndef NEWLINE
#define NEWLINE '\n'
#endif

char *gets(char *s) {
  char *p = s;
  int ch = 0;
  while (ch != EOL && ch != EOF) {
    ch = getchar();
    if (ch == EOL || ch == EOF) {
      putchar(NEWLINE);
      break;
    }
    *p++ = ch;
  }
  *p = '\0';
  return s;
}

int puts(const char *s) {
  while (*s) {
    putchar(*s++);
  }
  putchar(NEWLINE);
  return 0;
}

int printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  int len = 0;
  while (*format) {
    if (*format == '%') {
      format++;
      switch (*format) {
      case 's': {
        const char *s = va_arg(args, const char *);
        while (*s) {
          putchar(*s++);
          len++;
        }
        break;
      }
      case 'x':
      case 'X': {
        const char *lut =
            *format == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";
        unsigned int n = va_arg(args, unsigned int);
        for (int i = 0; i < 4; i++) {
          putchar(lut[(n >> (12 - i * 4)) & 0xf]);
          len++;
        }
        break;
      }
      }
    } else {
      putchar(*format);
      len++;
    }
    format++;
  }

  va_end(args);
  return len;
}
