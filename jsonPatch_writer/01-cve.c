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
	// int a2;
} __attribute__((__packed__, aligned(4))) stack_frame;

static void register_functions(struct ubpf_vm *vm);
char *read_json(const char *json);
void getEbpfPatch(const char *patchName);

static int8_t *bytecode;
uint64_t bytecode_len;
int test_global = 2022;

int orig_c0(int v)
{
	if (v > 2000)
	{
		return 1;
	}
	return 0;
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
	// jit
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
	// TODO: 能不能在这里将sp传递到frame中呢？？？怎么生成一个全面（能够对其成员进行赋值，不存在的就置空？）的frame？
	__asm__ __volatile__(
		"push %rbp \n\t"

		// save arguments to stack
		"sub $0x8,%rsp \n\t"
		"mov %rsp,%rbp \n\t"
		"mov %rdi,0x00(%rbp) \n\t"
		//"mov %rsi,0x08(%rbp) \n\t"
		//"mov %rdx,0x10(%rbp) \n\t"
		//"mov %rcx,0x18(%rbp) \n\t"
		//"mov %r8,0x20(%rbp) \n\t"
		//"mov %r9,0x28(%rbp) \n\t"

		// patch_dispatcher(stack_pointer)
		"mov %rbp, %rdi \n\t" // arg1 = sp
		"callq patch_dispatcher \n\t"

		// // restore context
		// "mov 0x00(%rbp), %rbx \n\t"
		// "mov 0x08(%rbp), %r12 \n\t"
		// "mov 0x10(%rbp), %r13 \n\t"
		// "mov 0x18(%rbp), %r14 \n\t"
		// "mov 0x20(%rbp), %r15 \n\t"
		// "mov 0x28(%rbp), %rbp \n\t"

		"mov %rbp, %rsp \n\t"
		"add $0x8,%rsp \n\t" // 16 byte aligned
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
	// ubpf_set_unwind_function_index(vm, 5);
}

// for hook
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
	// 目前是直接就解析json文件，而不是漏洞函数执行时再解析
	getEbpfPatch(strcat(argv[0], ".patch.json"));
	hook2(orig_c0, patch_handler);

	if (orig_c0(3000) == 10)
	{
		printf("This is New Function\n");
	}
	else if (orig_c0(3000) == 1)
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
	// 	result = orig_c0(3000);
	// 	if (result == 10)
	// 	{
	// 		printf("This is New Function\n");
	// 	}
	// 	else if (result == 1)
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