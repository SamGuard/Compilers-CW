function a(){
  int b = 0;
  function f(){
    b = b + 1;
    return b;
  } 
  return f;
}

int main(){
  function f = a();
  f();
  f();
  f();
  f();
  return f();
}