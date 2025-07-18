//pointer to array declaration
void fun(int **i) {
    int a;
    int (*b)[10];
    (*b)[0] = 1;
    a = (*b)[0];
}
int main(int argc) {
    int *(*m);
    fun(m)
}
