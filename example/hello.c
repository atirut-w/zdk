int bdos(int func, int arg);

int main() {
  bdos(2, 'H');
  bdos(2, 'e');
  bdos(2, 'l');
  bdos(2, 'l');
  bdos(2, 'o');
  bdos(2, '!');
  bdos(2, '\n');
  return 0;
}

void start() {
  main();
  bdos(0, 0);
}
