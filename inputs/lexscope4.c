int getFuncFunc(){
  int a = 1;
  function getFunc(){
    int b = 1;
    int f(){
      return a + b;
    }
    return f;
  }
  return getFunc();
}

int main(){
  function f = getFuncFunc();
  return f();
}