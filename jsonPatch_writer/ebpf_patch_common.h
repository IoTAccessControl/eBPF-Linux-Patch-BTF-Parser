
// macro for reading params
#define REGS_SP(x) ((x)->sp)
#define REGS_PARAM(x, index) \
({ \
	unsigned long ret; \
	switch (index) \
	{ \
	case 1: \
		ret = (x)->di; \
		break; \
	case 2: \
		ret = (x)->si; \
		break; \
	case 3: \
		ret = (x)->dx; \
		break; \
	case 4: \
		ret = (x)->cx; \
		break; \
	case 5: \
		ret = (x)->r8; \
		break; \
	case 6: \
		ret = (x)->r9; \
		break; \
	default: \
		ret = 0; \
	} \
ret; })

// stack frame for func call
typedef struct Stack_frame
{
	unsigned long di;
	unsigned long si;
	unsigned long dx;
	unsigned long cx;
	unsigned long r8;
	unsigned long r9;
	unsigned long sp;
} __attribute__((__packed__, aligned(4))) stack_frame;

// eBPF helper
static uint32_t (*new_sqrt)(uint32_t x) = (void *)1;
int (*strcmp_ext)(const char *s1, const char *s2) = (void *)2;
static uint64_t (*unwind)(uint64_t i) = (void *)3;

// CALL_C_FUNCTION()