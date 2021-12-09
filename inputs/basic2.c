int get1(int x){
  int a = 1;
  return x + a;
}

int main(){
  int a = 2;
  int b = get1(a);
  a = a + b;
  return a;
}