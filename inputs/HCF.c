int hcf(int a, int b){
  int t;
  while (b != 0) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}


int main(){
  return hcf(44, 36);
}