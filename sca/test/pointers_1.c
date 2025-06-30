// Pointers
void fun(int i) {
    int a = 2*3 ;
    int b = 2;
    int c;
    int sum = 0;
    int d = 0;
    int e = 0;
    int g = 0;
    int **p;
    int *q;
    *p = *q;
    sum = a*b+c ;
    while(2) {
        sum = a+b+c+*(*p);
        c=2;
    }
    *(*p) = *q;
    if( a+b ) {
        b=2;
        sum = a+d;
        if( a+g ) {
            sum = a+c;
        }
        else {
            sum = a+e;
        }
    }
    else{
        int b;
        sum = b+c+*(*p);
    }
}
int main(int argc) {
    int m=0;
    fun(m) ;
}
