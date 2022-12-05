/*
note: "The OPENSSL_LH_flush() function, which empties a hash table, contains a bug that breaks reuse of the memory occuppied by the removed hash table entries."
cve link: https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2022-1473
commit link: https://github.com/openssl/openssl/commit/64c85430f95200b6b51fe9475bd5203f7c19daf1
*/

#define NULL 0

#include "04-cve.bpf.h"

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