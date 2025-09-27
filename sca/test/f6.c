// Code snippet - 6

void f6() {
    int *p=malloc(4);
    int *q;
    int a;
    q = p;
    free(p);
    a = *q;
}       
int main() {
    f6();
}