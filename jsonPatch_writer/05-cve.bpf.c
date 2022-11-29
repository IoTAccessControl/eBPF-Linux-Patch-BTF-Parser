/*
note: "Fixes use of freed memory." BY adding memset().
cve link: https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2015-3417
commit link: https://github.com/FFmpeg/FFmpeg/commit/e8714f6f93d1a32f4e4655209960afcf4c185214
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "05-cve.bpf.h"

// void dummy_cve_2015_3417_ff_h264_free_tables(H264Context *h, int free_rbsp) {
void dummy_cve_2015_3417_ff_h264_free_tables(stack_frame *ctx)
{
	// stack_frame *frame = (stack_frame *)mem;

	H264Context *h = (H264Context *)REGS_PARM1(ctx);
	int free_rbsp = REGS_PARM2(ctx);

	// int i;
	if (free_rbsp && h->DPB)
	{
		/*for (i = 0; i < 36; i++)
			printf("1");*/
		// patch
		memset(h->delayed_pic, 0, sizeof(h->delayed_pic));
		// av_freep(&h->DPB);
	}
	else if (h->DPB)
	{
		// for (i = 0; i < 36; i++)
		h->DPB[0].needs_realloc = 36 + free_rbsp;
	}
}
