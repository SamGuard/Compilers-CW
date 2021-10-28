int loop(int x){
  while(1){
    if(x != 20){
      x = x + 1;
    } else {
      break;
      x = x - 1;
    }
  }
  return x;
  x = x + 1;
}

int main(){
  int x = 10;
  loop(x);
}