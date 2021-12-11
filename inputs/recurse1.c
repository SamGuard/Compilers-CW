int recurse(int x){
  int out = 0;
  if(1 < x){
    out = recurse(x-1) + 1;
  }
  return out;
}

int main(){
  int a = 5;
  int x = recurse(a);
  return x;
}