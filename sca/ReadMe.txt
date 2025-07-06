DEFCHECKER

DefChecker is a static code analyzer that analyzes C code and identifies uninitialized variables and undefined pointer variable access. 

Parses C file and builds Control Flow Graph (CFG).
Performs "Use before definition" check on the CFG and produces list of undefined variables as output.

Built using Qt 6.6.1 MinGW 11.2.

Source code is available at sca\analyzer\CodeAnalyzer
Test files are available at sca\test

1
int b;
int c=5;
int sum = b+c;

Undefined variable b

2
int b;
int c=5;
if( a+b ) {
    b=2;
}
else {
    int sum=b+c;
}

Undefined variable b
Variable b is not assigned value in else case

3
int b;
int c=5;
if( a+b ) {
    b=2;
}
else {
    int b=6;
    int sum=b+c;
}

No error
Variable b in else block shadows the uninitialized variable in outer scope

4
int *q;
int a = *q;

Undefined ptr q

5
int *p=malloc(4);
int *q;
q = p;
int a = *q;

No error
Variables p and q both point to the same memory allocated by malloc

6
int *p=malloc(4);
int *q;
q = p;
delete(p);
int a = *q;

Undefined ptr q