#include<cstdio>

int allocate(int *r) {
	int *q;
	int c=2;
	int b;
	b=c;
	if(r) {
		q = malloc(8);
	}
	else {
		int j;
	}
	free(q);
	*r = b;
	r=q;
}


int main() {
	int *p = malloc(8);
	int a;
	allocate(p);
	a = *p;
}