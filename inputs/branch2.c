int main(){
  int a = 101;
  int b = 100;
  if(a - b < 1 ){
    a = a + b;
  } else {
    b = b - a;
  }
  return 0;
}