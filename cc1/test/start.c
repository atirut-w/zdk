typedef unsigned short size_t;

size_t read(unsigned char dev, void *buf, size_t size);
size_t write(unsigned char dev, const void *buf, size_t size);
void exit(int code);

void puts(const char *str) {
  const char *p = str;
  size_t len = 0;
  while (*p != 0) {
    len = len + 1;
    p = p + 1;
  }
  write(0, str, len);
}

unsigned char retchar(unsigned char c) { return c; }

void start() {
  if (retchar('A') == 'A') {
    puts("Test passed.\n");
    exit(0);
  }
  puts("Test failed.\n");
  exit(1);
}
