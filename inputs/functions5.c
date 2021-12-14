int add1(int x){
  return x + 1;
}

int add2(int x){
  return add1(add1(x));
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