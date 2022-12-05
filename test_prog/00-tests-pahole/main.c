#include <stdio.h>
#include <stdlib.h>

/*
测试hash 冲突
*/

unsigned short BKDRHash(char *str)
{
	unsigned short seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned short hash = 0;
	while (*str)
	{
		hash = hash * seed + (*str++);
	}
	return (hash & 0xFFFF);
}

unsigned short APHash(char *str)
{
	unsigned short hash = 0;
	int i;
	for (i=0; *str; i++)
	{
		if ((i & 1) == 0)
		{
			hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
		}
		else
		{
			hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
		}
	}
	return (hash & 0xFFFF);
}

unsigned short DJBHash(char *str)
{
	unsigned short hash = 5381;
	while (*str)
	{
		hash += (hash << 5) + (*str++);
	}
	return (hash & 0xFFFF);
}

unsigned short JSHash(char *str)
{
	unsigned int hash = 1315423911;
	while (*str)
	{
		hash ^= ((hash << 5) + (*str++) + (hash >> 2));
	}
	return (hash & 0xFFFF);
}

#define ANONY_HASH_FUNC_NUM 4
typedef unsigned short (*anony_str_hash_func)(char *str);
anony_str_hash_func anony_str_hash[ANONY_HASH_FUNC_NUM] = {BKDRHash, APHash, DJBHash, JSHash};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: test [str]\n");
		exit(-1);
	}

	char *raw_str = argv[1];
	printf("raw str: %s\n", raw_str);

	for (int i = 0; i < ANONY_HASH_FUNC_NUM; i++) {
		printf("hash[%d] -> %d\n", i, anony_str_hash[i](raw_str));
	}
	return 0;
}

/* 测试用例
modern_browsers 12555|37128
raw str: modern_browsers
hash[0] -> 12555
hash[1] -> 37128
hash[2] -> 54464
hash[3] -> 29627

len_hi 12555
raw str: len_hi
hash[0] -> 12555
hash[1] -> 5676
hash[2] -> 50996
hash[3] -> 39432
*/