int f(function func, int arg){
  return func(arg) + 1;
}

int main(){
  function a(int x){
    return x + 123;
  }
  return f(a, 2);
}