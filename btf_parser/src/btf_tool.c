#include "btf_tool.h"

int test(struct A *t)
{
	return t->a;
}

int vul_func(int v)
{
	if (v > 2000) {
		return 1;
	}
	return 0;
}
