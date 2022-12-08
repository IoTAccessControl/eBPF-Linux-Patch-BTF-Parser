#ifndef LIB_TEST_H_
#define LIB_TEST_H_

long libtest_version();

int libtest_add(int a, int b);

struct A {
	char b;
	int  a;
};
int libtest_test(struct A *t);

#endif
