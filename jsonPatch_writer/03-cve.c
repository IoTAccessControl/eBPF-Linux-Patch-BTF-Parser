#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>

#include "cJSON.h"
#include "ubpf_int.h"

typedef struct stack_frame
{
	uint64_t a1;
	uint64_t a2;
} __attribute__((__packed__, aligned(4))) stack_frame;

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

static void register_functions(struct ubpf_vm *vm);
char *read_json(const char *json);
void getEbpfPatch(const char *patchName);

static int8_t *bytecode;
uint64_t bytecode_len;

int orig_c0(long *t, apr_time_exp_t *xt)
{
	long year = xt->tm_year;
	// long days;
	// static const int dayoffset[12] = {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

	if (xt->tm_mon < 2)
		year--;

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

char *read_json(const char *json)
{
	FILE *fp;
	fp = fopen(json, "r");
	if (fp == NULL)
	{
		printf("fail to open the file\n");
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
	getEbpfPatch(strcat(argv[0], ".patch.json"));
	hook2(orig_c0, patch_handler);

	long *t = NULL;
	apr_time_exp_t *xt;
	xt = (apr_time_exp_t *)malloc(sizeof(apr_time_exp_t));
	xt->tm_mon = -1;
	xt->tm_year = 1222;

	if (orig_c0(t, xt) == 20004)
	{
		printf("This is New Function\n");
	}
	else if (orig_c0(t, xt) == 20000)
	{
		printf("This is Old Function\n");
	}
	else
	{
		printf("something wrong.\n");
	}

	/* THIS IS FOR PTRACE HOOK */
	// int result = 0;
	// int i = 0;
	// while (1)
	// {
	// 	printf("tick: %d\n", i);
	// 	printf("Demo! [pid:%d]\n", getpid());
	// 	result = orig_c0(t, xt);
	// 	if (result == 20004)
	// 	{
	// 		printf("This is New Function\n");
	// 	}
	// 	else if (result == 0)
	// 	{
	// 		printf("This is Old Function\n");
	// 	}
	// 	else
	// 	{
	// 		printf("something wrong.\n");
	// 	}

	// 	i++;
	// 	sleep(2);
	// }

	return 0;
}