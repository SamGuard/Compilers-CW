int main(){
  int a = 101;
  int b = 100;
  if(a - b < 1 ){
    b = a + b;
  } else {
    b = b - a;
  }

  return b;
}