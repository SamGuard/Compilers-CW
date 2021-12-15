int x = 3;
int f(function z){
  int x = 100;
  return z(1);
}

int main() {
    int x = 10;
    int g(int y) { return x + y; }
    return f(g);
}