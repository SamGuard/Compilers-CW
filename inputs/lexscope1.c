int f(function func, int arg){
  return func(arg) + 1;
}

int main(){
  function a(int x){
    a = 123;
    return x + a;
  }
  return f(a, 2);
}