#ifndef __BOOTSTRAP_H
#define __BOOTSTRAP_H

#define EVP_PKT_SIGN    0x0010

typedef struct Stack_frame {
	int *a1;
	//uint64_t a2;
	//uint64_t a3;
	//uint64_t a4;
	//uint64_t a5;
	//uint64_t a6;
} __attribute__ ((__packed__, aligned(4))) stack_frame;

#endif /* __BOOTSTRAP_H */