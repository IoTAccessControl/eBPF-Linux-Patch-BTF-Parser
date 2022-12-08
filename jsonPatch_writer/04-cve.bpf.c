/*
note: "The OPENSSL_LH_flush() function, which empties a hash table, contains a bug that breaks reuse of the memory occuppied by the removed hash table entries."
cve link: https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2022-1473
commit link: https://github.com/openssl/openssl/commit/64c85430f95200b6b51fe9475bd5203f7c19daf1
*/

#include <stdint.h>
#include "ebpf_patch_common.h"

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

#define NULL ((void *)0)

int eBPF_Patch(stack_frame *ctx)
{
	OPENSSL_LHASH *lh = (OPENSSL_LHASH *)REGS_PARAM(ctx, 1);
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
			// free(n); // ??
			n = nn;
		}
		lh->b[i] = NULL;
	}
	lh->num_items = 0;

	return 0;
}