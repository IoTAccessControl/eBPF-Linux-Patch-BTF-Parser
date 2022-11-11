#ifndef __BOOTSTRAP_H
#define __BOOTSTRAP_H

typedef struct Stack_frame {
	uint64_t a1;
	uint64_t a2;
	//uint64_t a3;
} __attribute__ ((__packed__, aligned(4))) stack_frame;

typedef struct Apr_time_exp_t {
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

#endif /* __BOOTSTRAP_H */
