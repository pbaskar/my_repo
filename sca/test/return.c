// Pointers - side effects on returning from other functions
void fun(int **i) {
    int a = 2*3 ;
    int b = 2;
    int c;
    int sum = 0;
    int d = 0;
    int e = 0;
    int g = 0;
    int **p;
    int *q;
    int *r;
    p = i;
    sum = a*b+c ;
    while(2) {
        sum = a+b+c+*(*i);
        c=2;
    }
    *q = *r;
    *i = q;
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
        sum = b+c+*(*i);
    }
    return;
}
int main(int argc) {
    int **m=malloc(8);
    int *k;
    fun(m) ;
    delete(m);
    k =*(*m);
}
