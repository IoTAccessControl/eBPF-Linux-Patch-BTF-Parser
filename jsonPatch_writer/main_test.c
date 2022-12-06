/* 集合cve测试 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>

#include "cJSON.h"
#include "ubpf_int.h"

typedef struct Stack_frame
{
	int a1;
} __attribute__((__packed__, aligned(4))) stack_frame;

static void register_functions(struct ubpf_vm *vm);
char *read_json(const char *json);
void getEbpfPatch(const char *patchName);

static int8_t *bytecode;
uint64_t bytecode_len;

/* CVE_1 */
typedef struct Test_For_Reloc
{
	int a1;
	// Add field c3
	int c3;
	int b2;
} test_for_reloc;

int cve_1(test_for_reloc *test)
{
	if (test->b2 > 2000)
	{
		printf("This is the bug version. Bug is here: %d.\n", __LINE__);
		return 1;
	}
	return 0;
}

/* CVE_2 */
#define EVP_PKT_SIGN 0x0010

int cve_2(int *s)
{
	int type = 1;
	int *peer; //, value = 11;
	peer = s;

	if ((peer != NULL) && (type & EVP_PKT_SIGN))
		return 1;
	printf("This is the bug version. Bug is here: %d.\n", __LINE__);
	return 0;
}

/* CVE_3 */
typedef struct apr_time_exp_t
{
	/** microseconds past tm_sec */
	int tm_usec;
	/** (0-61) seconds past tm_min */
	int tm_sec;
	/** (0-59) minutes past tm_hour */
	int tm_min;
	/** (0-23) hours past midnight */
	int tm_hour;
	/** (1-31) day of the month */
	int tm_mday;
	/** (0-11) month of the year */
	int tm_mon;
	/** year since 1900 */
	int tm_year;
	/** (0-6) days since Sunday */
	int tm_wday;
	/** (0-365) days since January 1 */
	int tm_yday;
	/** daylight saving time */
	int tm_isdst;
	/** seconds east of UTC */
	int tm_gmtoff;
} apr_time_exp_t;

int cve_3(long *t, apr_time_exp_t *xt)
{
	long year = xt->tm_year;
	// long days;
	// static const int dayoffset[12] = {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

	if (xt->tm_mon < 2)
		year--;
	printf("This is the bug version. Bug is here: %d.\n", __LINE__);
	return 20000;

	// days = year * 365 + year / 4 - year / 100 + (year / 100 + 3) / 4;
	// days += dayoffset[xt->tm_mon] + xt->tm_mday - 1;
	// days -= 25508; /* 1 jan 1970 is 25508 days since 1 mar 1900 */
	// days = ((days * 24 + xt->tm_hour) * 60 + xt->tm_min) * 60 + xt->tm_sec;

	// if (days < 0)
	// {
	// 	return 20000;
	// }
	// *t = days * (long)1000000 + xt->tm_usec;

	// return 0;
}

/* CVE_4 */
struct lhash_node_st
{
	void *data;
	struct lhash_node_st *next;
	unsigned long hash;
};

typedef struct lhash_st OPENSSL_LHASH;
typedef struct lhash_node_st OPENSSL_LH_NODE;
typedef int (*OPENSSL_LH_COMPFUNC)(const void *, const void *);
typedef unsigned long (*OPENSSL_LH_HASHFUNC)(const void *);

struct lhash_st
{
	OPENSSL_LH_NODE **b;
	OPENSSL_LH_COMPFUNC comp;
	OPENSSL_LH_HASHFUNC hash;
	unsigned int num_nodes;
	unsigned int num_alloc_nodes;
	unsigned int p;
	unsigned int pmax;
	unsigned long up_load;	 /* load times 256 */
	unsigned long down_load; /* load times 256 */
	unsigned long num_items;
	int error;
};

int cve_4(OPENSSL_LHASH *lh)
{
	unsigned int i;
	OPENSSL_LH_NODE *n, *nn;

	if (lh == NULL)
		return 0;

	for (i = 0; i < lh->num_nodes; i++)
	{
		n = lh->b[i];
		while (n != NULL)
		{
			nn = n->next;
			// OPENSSL_free(n);
			free(n); // ??
			n = nn;
		}
		lh->b[i] = NULL;
	}
	printf("This is the bug version. Bug is here: %d.\n", __LINE__);
	return lh->num_items;
}

/* CVE_5 */
typedef struct H264Picture
{
	int needs_realloc;
} H264Picture;

typedef struct H264Context
{
	H264Picture *DPB;
	H264Picture *delayed_pic[18];
} H264Context;

void cve_5(H264Context *h, int free_rbsp)
{
	// int i;
	if (free_rbsp && h->DPB)
	{
		// omit...
		// Patch
		// memset(h->delayed_pic, 0, sizeof(h->delayed_pic));
		printf("This is the bug version. Bug is here: %d.\n", __LINE__);
		h->DPB[0].needs_realloc = -1;
	}
	else if (h->DPB)
	{
		// omit...
	}
}

int run_ebpf(stack_frame *frame, const char *code, unsigned long code_len)
{
	struct ubpf_vm *vm = ubpf_create();
	if (!vm)
	{
		fprintf(stderr, "Failed to create VM\n");
		return 1;
	}

	register_functions(vm);
	vm->insts = malloc(code_len);
	if (vm->insts == NULL)
	{
		return -1;
	}
	memcpy(vm->insts, code, code_len);
	vm->num_insts = code_len / sizeof(vm->insts[0]);

	uint64_t ret;
	// JIT
	// char *errmsg;
	// ubpf_jit_fn fn = ubpf_compile(vm, &errmsg);
	// if (fn == NULL) {
	//     fprintf(stderr, "Failed to compile: %s\n", errmsg);
	//     free(errmsg);
	//     ret = UINT64_MAX;
	//     return ret;
	// }
	// ret = fn(frame, sizeof(*frame));

	// no jit
	if (ubpf_exec(vm, frame, sizeof(*frame), &ret) < 0)
		ret = UINT64_MAX;
	ubpf_destroy(vm);
	return ret;
}

__attribute__((naked)) void patch_handler()
{
	__asm__ __volatile__(
		"push %rbp \n\t"

		// save arguments to stack
		"sub $0x30,%rsp \n\t"
		"mov %rsp,%rbp \n\t"
		"mov %rdi,0x00(%rbp) \n\t"
		"mov %rsi,0x08(%rbp) \n\t"
		"mov %rdx,0x10(%rbp) \n\t"
		"mov %rcx,0x18(%rbp) \n\t"
		"mov %r8,0x20(%rbp) \n\t"
		"mov %r9,0x28(%rbp) \n\t"

		// patch_dispatcher(stack_pointer)
		"mov %rbp, %rdi \n\t" // arg1 = sp
		"callq patch_dispatcher \n\t"

		"mov %rbp, %rsp \n\t"
		"add $0x30,%rsp \n\t"
		"pop %rbp \n\t"
		"retq \n\t");
}

int patch_dispatcher(stack_frame *frame)
{
	// run patch
	int ret = run_ebpf(frame, (const char *)bytecode, bytecode_len);
	return ret;
}

static uint32_t new_sqrt(uint32_t x)
{
	return sqrt(x);
}

static uint64_t unwind(uint64_t i)
{
	return i;
}

static void register_functions(struct ubpf_vm *vm)
{
	ubpf_register(vm, 1, "new_sqrt", new_sqrt);
	ubpf_register(vm, 2, "strcmp_ext", strcmp);
	ubpf_register(vm, 3, "unwind", unwind);
}

// For hook
struct jmp
{
	uint32_t opcode : 8;
	int32_t offset : 32;
} __attribute__((packed));

#define JMP(off) ((struct jmp){0xe9, off - sizeof(struct jmp)})
#define PG_SIZE sysconf(_SC_PAGESIZE)

static inline bool within_page(void *addr)
{
	return (uintptr_t)addr % PG_SIZE + sizeof(struct jmp) <= PG_SIZE;
}

void hook2(void *old, void *new)
{
	void *base = (void *)((uintptr_t)old & ~(PG_SIZE - 1));
	size_t len = PG_SIZE * (within_page(old) ? 1 : 2);

	int flags = PROT_WRITE | PROT_READ | PROT_EXEC;
	if (mprotect(base, len, flags) == 0)
	{
		*(struct jmp *)old = JMP((char *)new - (char *)old); // **PATCH**
		mprotect(base, len, flags & ~PROT_WRITE);
	}
	else
	{
		perror("DSU fail\n");
	}
}

char *read_json(const char *json)
{
	FILE *fp;
	fp = fopen(json, "r");
	if (fp == NULL)
	{
		perror("fail to open the file\n");
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	char *data;
	data = (char *)malloc((len + 1) * sizeof(char));
	if (!data)
	{
		printf("fail to malloc\n");
		exit(-1);
	}
	rewind(fp);
	fread(data, 1, len, fp);
	data[len] = '\0';
	fclose(fp);
	return data;
}

void getEbpfPatch(const char *patchName)
{
	cJSON *cjson_data = NULL;
	cJSON *cjson_ebpf_patch = NULL;
	cJSON *cjson_ebpf_patch_len = NULL;

	/* 解析整段JSO数据 */
	char *json;
	json = read_json(patchName);
	cjson_data = cJSON_Parse(json);
	if (cjson_data == NULL)
	{
		printf("parse fail.\n");
		exit(-1);
	}

	cjson_ebpf_patch = cJSON_GetObjectItem(cjson_data, "ebpf_patch");
	cjson_ebpf_patch_len = cJSON_GetObjectItem(cjson_data, "ebpf_patch_len");

	bytecode_len = cjson_ebpf_patch_len->valueint;
	bytecode = (int8_t *)malloc(bytecode_len);
	char *token;
	char **endp = NULL;
	token = strtok(cjson_ebpf_patch->valuestring, "\\x");
	for (int i = 0; i < bytecode_len && token != NULL; i++)
	{
		bytecode[i] = (unsigned char)strtoul(token, endp, 16);
		token = strtok(NULL, "\\x");
	}

	cJSON_Delete(cjson_data);
	free(json);
}

int main(int argc, char **argv)
{
	char patch_name[64];
	enum CVE
	{
		CVE_1=1, CVE_2, CVE_3, CVE_4, CVE_5
	};

	/* 各CVE参数准备 */
	// CVE_1
	test_for_reloc *test = (test_for_reloc *)malloc(sizeof(test_for_reloc));
	test->b2 = 2500;
	test->c3 = 3500;
	// CVE_2
	// CVE_3
	long *t = NULL;
	apr_time_exp_t *xt;
	xt = (apr_time_exp_t *)malloc(sizeof(apr_time_exp_t));
	xt->tm_mon = -1;
	xt->tm_year = 2000;
	// CVE_4
	OPENSSL_LHASH *lh;
	lh = (OPENSSL_LHASH *)malloc(sizeof(OPENSSL_LHASH));
	lh->num_nodes = 0;
	lh->num_items = 123;
	// CVE_5
	int free_rbsp = 1;
	H264Context *h;
	H264Picture *dpb;
	dpb = (H264Picture *)malloc(sizeof(H264Picture));
	dpb->needs_realloc = 1;
	h = (H264Context *)malloc(sizeof(H264Context));
	h->DPB = dpb;
	h->delayed_pic[0] = dpb;
	h->delayed_pic[1] = dpb;

	/* 运行CVE */
	int ret;
	enum CVE cve;
	for (cve = CVE_1; cve <= CVE_5; cve++)
	{
		switch(cve)
		{
			case CVE_1:
				printf("====== CVE %d ======\n", CVE_1);
				ret = cve_1(test);
				printf("== Install Patch ==\n");
				hook2(cve_1, patch_handler);
				sprintf(patch_name, "bin/0%d-cve.patch.json", CVE_1);
				getEbpfPatch(patch_name);
				ret = cve_1(test);
				if (ret == 0) {printf("This is new version. Bug is fixed.\n");}
				else if(ret == 1) {printf("Bug is not fixed.\n");}
				break;
			case CVE_2:
				printf("====== CVE %d ======\n", CVE_2);
				ret = cve_2((int *)11);
				printf("== Install Patch ==\n");
				hook2(cve_2, patch_handler);
				sprintf(patch_name, "bin/0%d-cve.patch.json", CVE_2);
				getEbpfPatch(patch_name);
				ret = cve_2((int *)11);
				if (ret == 1) {printf("This is new version. Bug is fixed.\n");}
				else if(ret == 0) {printf("Bug is not fixed.\n");}
				break;
			case CVE_3:
				printf("====== CVE %d ======\n", CVE_3);
				ret = cve_3(t, xt);
				printf("== Install Patch ==\n");
				hook2(cve_3, patch_handler);
				sprintf(patch_name, "bin/0%d-cve.patch.json", CVE_3);
				getEbpfPatch(patch_name);
				ret = cve_3(t, xt);
				if (ret == 20004) {printf("This is new version. Bug is fixed.\n");}
				else if(ret == 20000) {printf("Bug is not fixed.\n");}
				break;
			case CVE_4:
				printf("====== CVE %d ======\n", CVE_4);
				ret = cve_4(lh);
				printf("== Install Patch ==\n");
				hook2(cve_4, patch_handler);
				sprintf(patch_name, "bin/0%d-cve.patch.json", CVE_4);
				getEbpfPatch(patch_name);
				ret = cve_4(lh);
				if (ret == 0) {printf("This is new version. Bug is fixed.\n");}
				else if(ret == 123) {printf("Bug is not fixed.\n");}
				break;
			case CVE_5:
				printf("====== CVE %d ======\n", CVE_5);
				cve_5(h, free_rbsp);
				printf("== Install Patch ==\n");
				hook2(cve_5, patch_handler);
				sprintf(patch_name, "bin/0%d-cve.patch.json", CVE_5);
				getEbpfPatch(patch_name);
				cve_5(h, free_rbsp);
				if (h->delayed_pic[0] == (H264Picture *)0) {printf("This is new version. Bug is fixed.\n");}
				else if(h->DPB[0].needs_realloc == -1) {printf("Bug is not fixed.\n");}
				break;
		}
	}

	return 0;
}