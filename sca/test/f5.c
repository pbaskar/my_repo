// Code snippet - 5

void f5() {
    int *p=malloc(4);
    int *q;
    int a;
    q = p;
    *p = 5;
    a = *q;
}       
int main() {
    f5();
}