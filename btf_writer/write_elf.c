#include <stdio.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <elf.h>

// BKDR Hash Function
unsigned int BKDRHash(const char *str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str) {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

// ONLY add a .FUNC section, lack other necessary ops
int add_section(const char *filename)
{
    // 打开文件
    FILE *fp; int fd;
    // const char *filename = argv[1];
    fp = fopen(filename, "r+");
    if (fp == NULL) {
        printf("fail to open the file\n");
        exit(-1);
    }

    // 解析head,判断elf文件类型
    Elf64_Ehdr elf_head;
    int ret = fread(&elf_head, sizeof(Elf64_Ehdr), 1, fp);
    if (ret == 0) {
        printf("fail to read head\n");
        exit(-1);
    }

    if (elf_head.e_ident[0] != 0x7F || elf_head.e_ident[1] != 'E' ||
        elf_head.e_ident[2] != 'L' || elf_head.e_ident[3] != 'F') {
        printf("Not a ELF file\n");
        exit(-1);
    }

    // 解析section 分配内存 section * 数量
    Elf64_Shdr *shdr = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr) * elf_head.e_shnum);
    if (shdr == NULL) {
        printf("shdr malloc failed\n");
        exit(-1);
    }
    // 设置fp偏移量 offset
    ret = fseek(fp, elf_head.e_shoff, SEEK_SET); // fseek调整指针的位置，采用参考位置+偏移量
    if (ret != 0) {
        printf("faile to fseek\n");
        exit(-1);
    }
    // 读取section 到 shdr, 大小为shdr * 数量
    ret = fread(shdr, sizeof(Elf64_Shdr) * elf_head.e_shnum, 1, fp);
    if (ret == 0) {
        printf("fail to read section\n");
        exit(-1);
    }

    // 重置指针位置到文件流开头
    rewind(fp);
    // 将fp指针移到 '字符串表' 偏移位置处
    fseek(fp, shdr[elf_head.e_shstrndx].sh_offset, SEEK_SET);
    // 第e_shstrndx项是字符串表 定义 字节 长度 char类型 数组
    char shstrtab[shdr[elf_head.e_shstrndx].sh_size];
    char *temp = shstrtab;
    // 读取内容
    ret = fread(shstrtab, shdr[elf_head.e_shstrndx].sh_size, 1, fp);
    if (ret == 0) {
        printf("faile to read\n");
        exit(-1);
    }

    // 遍历 1
    uint8_t *str_data;
    for (int i = 0; i < elf_head.e_shnum; i++) {
        temp = shstrtab;
        temp = temp + shdr[i].sh_name;

        if (strcmp(temp, ".strtab") != 0)
            continue;
        str_data = (uint8_t *)malloc(sizeof(uint8_t) * shdr[i].sh_size);
        fseek(fp, shdr[i].sh_offset, SEEK_SET);
        fread(str_data, sizeof(uint8_t) * shdr[i].sh_size, 1, fp);
    }

    // 遍历 2
    for (int i = 0; i < elf_head.e_shnum; i++) {
        temp = shstrtab;
        temp = temp + shdr[i].sh_name;

        if (strcmp(temp, ".symtab") != 0)
            continue;
        // printf("节的名称: %s\n", temp);
        // printf("节首的偏移: %lx\n", shdr[i].sh_offset);
        // printf("节的大小: %lx\n", shdr[i].sh_size);

        uint8_t *sign_data = (uint8_t *)malloc(sizeof(uint8_t) * shdr[i].sh_size);
        fseek(fp, shdr[i].sh_offset, SEEK_SET);
        fread(sign_data, sizeof(uint8_t) * shdr[i].sh_size, 1, fp);

        Elf64_Sym *psym = (Elf64_Sym *)sign_data;
        int ncount = shdr[i].sh_size / shdr[i].sh_entsize;

        // TODO:read and write here, get rid of some unnecessary operations
        for (int i = 0; i < ncount; ++i) {
            psym[i].st_name = BKDRHash(psym[i].st_name + str_data);
        }

        // add .FUNC section
        const char *llvm_objcopy;
        char tmp_fn[4096];
        char cmd[4096 * 2];
        llvm_objcopy = getenv("LLVM_OBJCOPY");
        if (!llvm_objcopy)
            llvm_objcopy = "llvm-objcopy";
        snprintf(tmp_fn, sizeof(tmp_fn), "%s.func", filename);
        fd = creat(tmp_fn, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            printf("open(%s) failed!\n", tmp_fn);
            exit(-1);
        }
        if (write(fd, sign_data, sizeof(uint8_t) * shdr[i].sh_size) != sizeof(uint8_t) * shdr[i].sh_size) {
            printf("%s: write of %ld bytes to '%s' failed\n",
                    __func__, sizeof(uint8_t) * shdr[i].sh_size, tmp_fn);
            goto unlink;
        }
        snprintf(cmd, sizeof(cmd), "%s --add-section .FUNC=%s %s",
                 llvm_objcopy, tmp_fn, filename);
        if (system(cmd)) {
            printf("%s: failed to add .FUNC section to '%s'\n",
                    __func__, filename);
            goto unlink;
        }
    unlink:
        unlink(tmp_fn);
        // fwrite(sign_data, sizeof(uint8_t) * shdr[i].sh_size, 1, fp);
    }
    if (fd != -1)
        close(fd);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    // 参数错误
    if (argc != 2) {
        printf("Usage: ./write_elf elf_path\n");
        exit(-1);
    }

    add_section(argv[1]);
    
    return 0;
}
