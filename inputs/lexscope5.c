int getFuncFunc(int x) {
    int a = 4;
    function f;
    if (x == 0) {
        function getFunc() {
            int b = 2;
            int f() { return a + b; }
            return f;
        }
        f = getFunc;
    } else {
        function getFunc2(){
          int b = 1;
            int g() { return a - b; }
            return g;
        }
        f = getFunc2;
    }
    return f();
}

int main() {
    function f = getFuncFunc(1);
    return f();
}