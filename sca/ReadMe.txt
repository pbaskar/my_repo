DEFCHECKER

DefChecker is a static code analyzer that analyzes C code.
Identifies uninitialized variables and undefined pointer variable access.

Parses C file and builds Control Flow Graph (CFG).
Performs "Use before definition" check on the variables in the assignments in CFG.
Produces list of undefined variables as output.

Source code is available at sca\analyzer\DefChecker

Test files are available at sca\test

Code snippets and expected result:

1) test\f1.c
Input:
    Line 1: void f1() {
    Line 2:     int b;
    Line 3:     int c=5;
    Line 4:     int sum = b+c;
    Line 5: }
    Line 6: int main() {
    Line 7:     f1();
    Line 8: }

Output:
    Undefined variable b in Line 4 : sum = b+c

2) test\f2.c
Input:
    Line 1:  void f2() {
    Line 2:      int a=1;
    Line 3:      int b;
    Line 4:      int c=5;
    Line 5:      if( a+c ) {
    Line 6:          b=2;
    Line 7:      }
    Line 8:      else {
    Line 9:          int sum=b+c;
    Line 10:      }
    Line 11: }
    Line 12: int main() {
    Line 13:     f2();
    Line 14: }

Output:
    Undefined variable b in Line 8 : sum=b+c

Comments:
    Variable b is not assigned a value in else case

3) test\f3.c
Input:
    Line 1:  void f3() {
    Line 2:      int a =1;
    Line 3:      int b;
    Line 4:      int c=5;
    Line 5:      if( a+c ) {
    Line 6:          b=2;
    Line 7:      }
    Line 8:      else {
    Line 9:          int b=6;
    Line 10:         int sum=b+c;
    Line 11:     }
    Line 12: }
    Line 13: int main() {
    Line 14:     f3();
    Line 15: }

Output:
    No error

Comments:
    Variable b in else block shadows the uninitialized variable in outer scope

4) test\f4.c
Input:
    Line 1: void f4() {
    Line 2:     int *q;
    Line 3:     int a = *q;
    Line 4: }
    Line 5: int main() {
    Line 6:     f4();
    Line 7: }

Output:
    Undefined variable q in Line 3 : a = *q
    Undefined variable *q in Line 3 : a = *q

5) test\f5.c
Input:
    Line 1: void f5() {
    Line 2:     int *p=malloc(4);
    Line 3:     int *q;
    Line 4:     int a;
    Line 5:     q = p;
    Line 6:     *p = 5;
    Line 7:     a = *q;
    Line 8: }
    Line 9: int main() {
    Line 10:     f5();
    Line 11:}

Output:
    No error

Comments:
    Variables p and q both point to the same memory allocated by malloc

6) test\f6.c
Input:
    Line 1: void f6() {
    Line 2:     int *p=malloc(4);
    Line 3:     int *q;
    Line 4:     int a;
    Line 5:     q = p;
    Line 6:     free(p);
    Line 7:     a = *q;
    Line 8: }
    Line 9: int main() {
    Line 10:     f6();
    Line 11:}

Output:
    Undefined variable q in Line 7 : a = *q
    Undefined variable *q in Line 7 : a = *q