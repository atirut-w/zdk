typedef unsigned short size_t;

size_t read(unsigned char dev, void *buf, size_t size);
size_t write(unsigned char dev, const void *buf, size_t size);
void exit(int code);

unsigned char retchar(unsigned char c) { return c; }

void start() {
  if (retchar('A') == 'A') {
    write(0, "Test passed.\n", 13);
    exit(0);
  }
  write(0, "Test failed.\n", 13);
  exit(1);
}
