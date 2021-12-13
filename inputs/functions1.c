int doNothing(){
  int a = 1;
  a = a + 1;
}

int main(){
  int a = 10;
  doNothing();
  return a;
}