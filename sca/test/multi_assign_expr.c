// comma separated assignment stmt *(*(p+1)+2)
void fun(int i) {
    int a = 2*3 ;
    int b;
    int **c = malloc(8);
    int sum = 0;
    int d = 0;
    int e = 0;
    int g = 0;
    int **p = malloc(8);
    int *q = malloc(8);

    *p = malloc(8);
    //*q = 6;
    *(*(p+1)+2) = *q;

    *c = malloc(8);
    //*(*c) = 5;
    a = 2, b=*(*(c+1)+2);

    sum = a*b+*(*c) ;
    b=2;

    if( a+b ) {
        sum = a+d;
        if( a+g ) {
            sum = a+*(*c);
        }
        else {
            sum = a+e;
        }
    }
    else{
        sum = b+*(*c);
    }
    while(2) {
        sum = a+b+*(*p);
    }
}
int main(int argc) {
    int m=0;
    fun(m) ;
}
