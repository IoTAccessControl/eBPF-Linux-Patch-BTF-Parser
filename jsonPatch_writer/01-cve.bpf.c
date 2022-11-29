/*
note: this is a dummy cve test
i.e. change the logic from "v>2000" to "v>3000"
*/
#include <stdint.h>
#include "01-cve.bpf.h"

int eBPF_Patch(stack_frame *ctx)
{
	if (REGS_PARM1(ctx) > 3000)
	{
		return 1;
	}
	return new_sqrt(100);
}
