#include <stdio.h>
// #include "wrapper.h"

extern int getFuncBTF(const char *elf_path, const char *func_name);

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: ./getBTF elf_path func_name\n");
        return -1;
    }

    getFuncBTF(argv[1], argv[2]);

    return 0;
}