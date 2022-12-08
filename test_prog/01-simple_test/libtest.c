#include "libtest.h"

long VERSION = 3;

long libtest_version() {
	return VERSION;
}

int libtest_add(int a, int b) {
	return a + b;
}

int libtest_test(struct A *t)
{
	return t->a;
}