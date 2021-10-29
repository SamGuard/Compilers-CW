int main(){
  int x = 0;
  int increm = 1;
  int add1(int x){ return x + increm; };
  
  while(x < 1000000){
    x = add1(x);
  }
  return x;
}