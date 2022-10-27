#include "libtest.h"
#include <stdio.h>

typedef int Val;

int buggy_func(Val a, Val b) {
	if (a < 2000) {
		return a + b;
	}
	return b;
}

int main(int argc, char *argv[]) {
	struct A tmp_1 = {'a', 11};
	int v = libtest_version();
	int t = argc;
	printf("version: %d add: %d\n", v, libtest_add(v, t));
	printf("buggy func: %d\n", buggy_func(2100, v));
	printf("struct A.a: %d\n", libtest_test(&tmp_1));
	return 0;
}