typedef unsigned short size_t;

size_t read(unsigned char dev, void* buf, size_t size);
size_t write(unsigned char dev, const void* buf, size_t size);
void exit(int code);

void start() {
  write(0, "Hello, World!\n", 14);
  exit(0);
}

