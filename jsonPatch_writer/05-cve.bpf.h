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
typedef struct h264Picture
{
	int needs_realloc;
} H264Picture;

typedef struct h264Context
{
	H264Picture *DPB;
	H264Picture *delayed_pic[18];
} H264Context;