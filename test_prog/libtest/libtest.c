#include "libtest.h"

int VERSION = 3;

int libtest_version() {
	return VERSION;
}

int libtest_add(int a, int b) {
	return a + b;
}

int libtest_test(struct A *t)
{
	return t->a;
}