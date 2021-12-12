int recurse(int x){
  if(1 < x){
    return recurse(x-1) + 1;
  }
  return 0;
}

int main(){
  int a = 5;
  int x = recurse(a);
  return x;
}