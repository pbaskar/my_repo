// comma separated assignment stmt *(*(p+1)+2)
void fun(int i) {
    int a = 2*3 ;
    int b;
    int **c;
    int sum = 0;
    int d = 0;
    int e = 0;
    int g = 0;
    int **p;
    int *q;
    a = 2, b=*(*(c+1)+2);
    *(*(p+1)+2) = *q;
    sum = a*b+c ;
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
        sum = b+c;
    }
    while(2) {
        sum = a+b+*p;
    }
}
int main(int argc) {
    int m=0;
    fun(m) ;
}
