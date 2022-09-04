#include "libtest.h"
#include <stdio.h>

int buggy_func(int a, int b) {
	if (a < 2000) {
		return a + b;
	}
	return b;
}

int main(int argc, char *argv[]) {
	int v = libtest_version();
	int t = argc;
	printf("version: %d add: %d\n", v, libtest_add(v, t));
	printf("buggy func: %d\n", buggy_func(2100, v));
	return 0;
}