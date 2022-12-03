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

typedef struct Apr_time_exp_t
{
	/** microseconds past tm_sec */
	int tm_usec;
	/** (0-61) seconds past tm_min */
	int tm_sec;
	/** (0-59) minutes past tm_hour */
	int tm_min;
	/** (0-23) hours past midnight */
	int tm_hour;
	/** (1-31) day of the month */
	int tm_mday;
	/** (0-11) month of the year */
	int tm_mon;
	/** year since 1900 */
	int tm_year;
	/** (0-6) days since Sunday */
	int tm_wday;
	/** (0-365) days since January 1 */
	int tm_yday;
	/** daylight saving time */
	int tm_isdst;
	/** seconds east of UTC */
	int tm_gmtoff;
} apr_time_exp_t;