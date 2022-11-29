#define REGS_SP(x) ((x)->sp)
#define REGS_PARM1(x) ((x)->di)
#define REGS_PARM2(x) ((x)->si)
#define REGS_PARM3(x) ((x)->dx)
#define REGS_PARM4(x) ((x)->cx)
#define REGS_PARM5(x) ((x)->r8)
// #define REGS_PARM5(x) ((x)->r9)

static uint32_t (*new_sqrt)(uint32_t x) = (void *)1;
int (*strcmp_ext)(const char *s1, const char *s2) = (void *)2;
static uint64_t (*unwind)(uint64_t i) = (void *)3;

typedef struct Stack_frame
{
	unsigned long di;
	unsigned long si;
	unsigned long dx;
	unsigned long cx;
	unsigned long r8;
	// unsigned long r9;
	// unsigned long sp;
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