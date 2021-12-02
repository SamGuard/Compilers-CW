int add1(int x){
  return x + 1;
}

int main() {
    int a = 2;

    int add2(int x) { return add1(add1(x)); };

    return add2(a);
}