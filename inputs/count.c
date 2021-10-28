int main(){
  int x = 0;
  int add1(int x){ return x + 1; }
  while(x < 1000000){
    x = add1(x);
  }
  return x;
}