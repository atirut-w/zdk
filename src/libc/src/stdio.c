#include <stdarg.h>
#include <stdio.h>

int puts(const char *s) {
  while (*s) {
    putchar(*s++);
  }
  putchar('\n');
  return 0;
}

int printf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  while (*format) {
    if (*format == '%') {
      format++;
      switch (*format) {
      case 'x':
      case 'X': {
        const char *lut =
            *format == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";
        unsigned int n = va_arg(args, unsigned int);
        for (int i = 7; i >= 0; i--) {
          putchar(lut[(n >> (i * 4)) & 0xf]);
        }
        break;
      }
      }
    } else {
      putchar(*format);
    }
    format++;
  }

  va_end(args);
  return 0;
}
