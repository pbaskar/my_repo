//struct declaration, definition, dot and pointer access operator
struct s {
    int a;
    int b;
}
int main(int argc) {
    int **m;
    struct s *s1;
    m=s1->a;
    m = (*s1).a;
    s1->a=1;
   (*s1).a = 2;
}