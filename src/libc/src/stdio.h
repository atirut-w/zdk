#ifndef _STDIO_H
#define _STDIO_H

#define EOF (-1)

// Unformatted I/O

int getchar(void);
char *gets(char *s);
int putchar(int c);
int puts(const char *s);

// Formatted I/O

int printf(const char *format, ...);

#endif
