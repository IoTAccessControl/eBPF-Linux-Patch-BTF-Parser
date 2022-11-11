
static uint32_t (*new_sqrt)(uint32_t x) = (void *)1;
int (*strcmp_ext)(const char *s1, const char *s2) = (void *)2;
static uint64_t (*unwind)(uint64_t i) = (void *)3;


typedef struct Stack_frame {
	int a1;
	// int a2;
} __attribute__ ((__packed__, aligned(4))) stack_frame;