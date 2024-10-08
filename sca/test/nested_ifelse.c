void fun(int i) {
    int a = 2*3 ;
    int b = 5;
    int c;
    int sum = 0;
    int d = 0;
    int e = 0;
    int g = 0;
    sum = a*b+c ;
    if( a+b ) {
        sum = a+d;
        if( a+g ) {
            sum = a+c;
        }
        else {
            sum = a+e;
        }
    }
    else{
        sum = b+c;
        if( a+g ) {
            sum = a+c;
        }
        else {
            sum = a+e;
        }
    }
    while(2) {
        sum = a+b+c;
    }
}
int main(int argc) {
    int m=0;
    fun(m) ;
}