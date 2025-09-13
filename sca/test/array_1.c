//array declaration
void fun(int **i) {
    int a;
    int b[10];
    a=b[0];
}
int main(int argc) {
    int **m=malloc(8);
    fun(m);
}
