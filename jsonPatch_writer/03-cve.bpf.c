/*
note: "out of bounds memory may be accessed in converting this value to an apr_time_exp_t value."
cve link: https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE_2017_12613
commit link: https://github.com/apache/apr/commit/da388844b8c48acc5f6e80f2689de6887b03745b
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "03-cve.bpf.h"

//int dummy_cve_2017_12613_apr_time_exp_get(long *t, apr_time_exp_t *xt) 
int dummy_cve_2017_12613_apr_time_exp_get(void *mem) 
{	
	stack_frame *frame = (stack_frame *)mem;
	long *t = (long *)(frame->a1);
	apr_time_exp_t *xt = (apr_time_exp_t *)(frame->a2);
	
	// 把long改成了unsigned long，因为不支持有符号除法
    unsigned long year = xt->tm_year;
    unsigned long days;
    static const int dayoffset[12] = {306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};
    // patch
    if (xt->tm_mon < 0 || xt->tm_mon >= 12)
        return 20004;
    if (xt->tm_mon < 2)
        year--;
    days = year * 365 + year / 4 - year / 100 + (year / 100 + 3) / 4;

    days = year * 365 + year / 4 - year / 100 + (year / 100 + 3) / 4;
    days += dayoffset[xt->tm_mon] + xt->tm_mday - 1;
    days -= 25508;              /* 1 jan 1970 is 25508 days since 1 mar 1900 */
    days = ((days * 24 + xt->tm_hour) * 60 + xt->tm_min) * 60 + xt->tm_sec;

    if (days < 0) {
        return 20000;
    }
    *t = days * (long)1000000 + xt->tm_usec;

    return 0;
}
