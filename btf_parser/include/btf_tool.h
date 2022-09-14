#include<stdio.h>

struct A {
	char b;
	int  a;
};

// int test(struct A *t);
// int vul_func(int v);
#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

int test(struct A *t);
int vul_func(int v);

#ifdef __cplusplus
}
#endif