/*
note: "Fix to prevent use of DH client certificates without sending certificate verify message." BY adding conditions.
cve link: https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2015-0205
commit link: https://github.com/openssl/openssl/commit/1421e0c584ae9120ca1b88098f13d6d2e90b83a3
*/

#include <stdio.h>
#include "02-cve.bpf.h"

int dummy_cve_2015_0205_ssl3_get_cert_verify(void *mem) {
    stack_frame *frame = (stack_frame *)mem;
    //int type = 1;
    int *peer;//, value = 11;
    peer = frame->a1;

    //if ((peer != NULL) && (type & EVP_PKT_SIGN))
    if (peer != NULL) //patch
        return 1;

    return 0;
}