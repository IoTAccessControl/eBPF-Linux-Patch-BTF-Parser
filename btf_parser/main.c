#include <stdio.h>
#include <string.h>
// #include "wrapper.h"

extern int getFuncBTF(const char *elf_path, const char *func_name, const char *flag);

int main(int argc, char **argv)
{
    if (argc == 3) {
        getFuncBTF(argv[1], argv[2], "all");
    } else if (argc == 4) {
        if (strcmp(argv[3], "single") == 0) {
            getFuncBTF(argv[1], argv[2], "single");
        }
        else {
            printf("Wrong args, single is not equal to %s\n", argv[3]);
            return -1;
        }
        
    } else {
        printf("Usage: ./getBTF elf_path func_name [single] (option single: print the specific funcinfo)\n");
        return -1;
    }

    return 0;
}
