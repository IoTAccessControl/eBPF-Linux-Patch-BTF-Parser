/*
note: "Fix to prevent use of DH client certificates without sending certificate verify message." BY adding conditions.
cve link: https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2015-0205
commit link: https://github.com/openssl/openssl/commit/1421e0c584ae9120ca1b88098f13d6d2e90b83a3
*/

#include <stdio.h>
#include <stdint.h>
#include "02-cve.bpf.h"

int eBPF_Patch(stack_frame *ctx)
{
	int *peer; //, value = 11;
	peer = (int *)REGS_PARAM(ctx, 1);

	// patch
	if (peer != NULL)
		return 1;

	return 0;
}