#include <stdint.h>
#include "01-cve.bpf.h"

int fix_func(void *mem)
{
    stack_frame *frame = (stack_frame *)mem;

    if (frame->a1 > 5000)
    {
        return 1;
    }
    return new_sqrt(100);
}
// Disassemble:
// ldxw r1, [r1]
// lsh r1, 0x20
// arsh r1, 0x20
// mov r0, 0x1
// jsgt r1, 0x1388, +1
// mov r0, 0x0
// exit