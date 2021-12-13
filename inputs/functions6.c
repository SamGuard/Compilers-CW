function getAdd(){
  function add1(int x){
    return x + 1;
  }
  return add1;
}

int main(){
  function f = getAdd();
  int a = 1;
  return f(a);
}