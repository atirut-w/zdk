int bdos(int func, int arg);

int main() {
  int ch = 'H';
  bdos(2, ch);
  ch = 'e';
  bdos(2, ch);
  ch = 'l';
  bdos(2, ch);
  ch = 'l';
  bdos(2, ch);
  ch = 'o';
  bdos(2, ch);
  ch = '\n';
  bdos(2, ch);
  return 0;
}

void start() {
  main();
  bdos(0, 0);
}
