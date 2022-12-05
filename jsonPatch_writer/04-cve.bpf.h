// 定义参数获取宏
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

// cve-related
typedef struct lhash_node_st
{
	void *data;
	struct lhash_node_st *next;
	unsigned long hash;
} OPENSSL_LH_NODE;

typedef int (*OPENSSL_LH_COMPFUNC)(const void *, const void *);
typedef unsigned long (*OPENSSL_LH_HASHFUNC)(const void *);

typedef struct lhash_st
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
} OPENSSL_LHASH;