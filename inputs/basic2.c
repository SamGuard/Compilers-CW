int add2(int x, int y, int z){
  return add1(x) + add1(x);
}

int add1(int x){
  return x + 1;
}

int add1(int x) { return x + 2; }

int main() {
    int a = 2;

    int add2(int x, int y, int z) { return x + y + z + 2; };

    return add2(a, 0, 1);
}