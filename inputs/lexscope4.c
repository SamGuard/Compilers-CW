int getFuncFunc(){
  int a = 4;
  function getFunc(){
    int b = 2;
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