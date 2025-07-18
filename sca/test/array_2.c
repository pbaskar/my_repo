//two dimensional array declaration
void fun(int **i) {
    int a;
    int b[10][10];
    b[0][0] = 1;
    a = b[0][0];
}
int main(int argc) {
    int *(*m);
    fun(m)
}
