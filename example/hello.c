int bdos(int func, int arg);

int main() {
  const char *msg = "Hello!\n";
  const char *p = msg;
  
  bdos(2, *p++);
  bdos(2, *p++);
  bdos(2, *p++);
  bdos(2, *p++);
  bdos(2, *p++);
  bdos(2, *p++);
  bdos(2, *p++);

  return 0;
}

void start() {
  main();
  bdos(0, 0);
}
