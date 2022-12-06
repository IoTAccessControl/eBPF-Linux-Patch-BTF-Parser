/*
note: this is a dummy cve test
i.e. change the logic from "v>2000" to "v>3000"
*/
#include <stdint.h>
// #include "01-cve.bpf.h"
#include "ebpf_patch_common.h"

typedef struct Test_For_Reloc
{
	int a1;
	int b2;
} test_for_reloc;

int eBPF_Patch(stack_frame *ctx)
{
	// patch
	test_for_reloc *test = (test_for_reloc *)REGS_PARAM(ctx, 1);
	if (test->b2 > 3000)
	{
		return 1;
	}
	return new_sqrt(0);
}
