#define REGS_SP(x) ((x)->sp)
#define REGS_PARM1(x) ((x)->di)
#define REGS_PARM2(x) ((x)->si)
#define REGS_PARM3(x) ((x)->dx)
#define REGS_PARM4(x) ((x)->cx)
#define REGS_PARM5(x) ((x)->r8)
// #define REGS_PARM5(x) ((x)->r9)

// static uint32_t (*new_sqrt)(uint32_t x) = (void *)1;
// int (*strcmp_ext)(const char *s1, const char *s2) = (void *)2;
// static uint64_t (*unwind)(uint64_t i) = (void *)3;

typedef struct Stack_frame
{
	unsigned long di;
	unsigned long si;
	unsigned long dx;
	unsigned long cx;
	unsigned long r8;
	// unsigned long r9;
	// unsigned long sp;
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