
int main(){
  int x = 0;
  int y = 0;

  while(x < 100000){
    y = 0;
    while(y < 100001){
      y = y + 1;
    }
    x = x + 1;
  }

  return x + y;
}