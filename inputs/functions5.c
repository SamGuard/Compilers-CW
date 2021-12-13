int add1(int x){
  return x + 1;
}

int add2(int x){
  return x + 2;
}

int main(){
  function f;
  int a = 1;

  if(a == 1){
    f = add2;
  } else {
    f = add1;
  }

  return f(a);
}