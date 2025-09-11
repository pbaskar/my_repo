// if with no else
void fun(int i) {
    int a = 2*3 ;
    int b;
    int c = 4;
    int sum = 0;
    int d = 0;
    int e = 0;
    int g = 0;
    int *p;
    int *q=malloc(8);
    p = q;
    *q = 4;
    sum = a*b+c ;
    if( a+b ) {
        b=2;
        sum = a+d;
    }
    else{
        int b;
        b=3;
        sum = b+c;
        if( a+g ) {
            sum = a+c;
        }
    }
    while(2) {
        sum = a+b+*p;
    }
}
int main(int argc) {
    int m=0;
    fun(m) ;
}
