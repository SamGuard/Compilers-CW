int factorial(int x) {
    if (x == 0)
        return 1;
    else
        return x * factorial(x - 1);
}

int main() {
  int x;
  readInt(x);
  return factorial(10);
}