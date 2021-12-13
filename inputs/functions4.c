int add1(int x){
  return x + 1;
}

int main(){
  function f = add1;
  int a = 1;
  return f(a);
}