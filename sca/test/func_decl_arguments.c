//Handle struct, function pointers as func decl arguments
struct t {
	int i;
}
t func(struct t t1,struct t r1) {
	t1 = f1(r1);
	return t1;
}