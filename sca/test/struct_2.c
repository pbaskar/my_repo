//pointer inside struct
struct s {
    int *a;
    int b;
}
int main(int argc) {
    int **m;
    struct s *s1;
    m=s1->a;
    (*s1).a = 1;
}
