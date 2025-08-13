#include<cstdio>

void swap(int *p, int *q) {
	int t;
	t = *p , *p = *q;
	*q = t;
	return;
}

int main() {
	int a=1;
	int b;
	int *r;
	int *s;
	r= &a;
	s= &b;
	swap(r,s);
}