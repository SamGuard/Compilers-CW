int get1(){
  int a = 1000;
  return a;
}

int main(){
  int a = 2;
  int b = get1();
  a = a + b;
  return a;
}