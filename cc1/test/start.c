typedef unsigned short size_t;

size_t read(unsigned char dev, void *buf, size_t size);
size_t write(unsigned char dev, const void *buf, size_t size);
void exit(int code);

void puthex8(unsigned char v);
void puthex16(unsigned short v);
void putnl(void);

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
  {
    unsigned short x;
    unsigned short *p = &x;
    if (p != &x) {
      puts("Address-of failed.\n");
      exit(2);
    }

    *p = 0x1234;
    if (x != 0x1234) {
      puts("Dereference failed.\n");
      puts("x = ");
      puthex16(x);
      putnl();
      puts("expected = 0x1234\n");
      exit(3);
    }
  }

  if (retchar('A') != 'A') {
    puts("Test failed.\n");
    exit(1);
  }

  puts("Test passed.\n");
  puts("retchar('A') = ");
  puthex8(retchar('A'));
  putnl();
  puts("expected = ");
  puthex16('A');
  putnl();
  exit(0);
}
