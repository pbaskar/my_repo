//struct inside struct
struct s {
    int a;
    int b;
}
struct t {
    int c;
    int d;
    struct s s1;
}
int main(int argc) {
    int m;
    struct t *t1;
    m=t1->d;
    (*t1).c = 1;
}