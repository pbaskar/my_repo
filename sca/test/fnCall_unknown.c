//function call whose function declaration is unknown

int func() {
	int a=0;
	int b;
	int c=0;
	c = sum(a,b);
	return c;
}

int main() {
	func();
}

