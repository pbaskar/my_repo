#include<cstdio>

int allocate(int *p) {
	int *q;
	if(p) {
		q = malloc(8);
	}
	else {
		int j;
	}
	delete(q);
	p = q;
}


int main() {
	int *p = malloc(8);
	allocate(p);
}