int main(){
  int a = 0;
  int b = 100;
  if(a - b < 0){
    a = a + b;
  } else {
    b = b - a;
  }
  return 0;
}