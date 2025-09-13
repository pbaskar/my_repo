// & operator
void fun(int **i) {
    int a = 2*3 ;
    int b = 2;
    int c = 3;
    int sum = 0;
    int d = 0;
    int e = 0;
    int g = 0;
    int **p;
    int *q = malloc(8);
    int *r = malloc(8);
    *r = 9;
    p = i;
    sum = a*b+c ;
    while(2) {
        int *c;
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
}
int main(int argc) {
    int a;
    int **k=malloc(8);
    int **m=*(&k);
    fun(m) ;
    free(k);
    a =*(*m);
}
