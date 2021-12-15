int main(){
  int a = 1;
  int b = 2;
  int c = 3;
  int d = 25;

  if(1 != 1){
    return 1;
  }
  if(a + b != 3){
    return 2;
  }
  if(a - b != 0-1){
    return 3;
  }
  if(b * c != 6){
    return 4;
  }
  if(c / b != 1){
    return 5;
  }
  if(d % c != 1){
    return 6;
  }
  if(c > b != 1){
    return 7;
  }
  if(b < c != 1){
    return 8;
  }
  if(b <= 2 != 1){
    return 9;
  }
  if(b >= 3 != 0){
    return 10;
  }

  return 0;
}