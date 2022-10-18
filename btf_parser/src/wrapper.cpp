#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <fcntl.h>

#include "btfparse/ibtf.h"
#include "wrapper.h"

bool parseTypes(const std::filesystem::path &filePath, const std::string &funcName);

int dumpAndParse(const char *elf_path, const char *func_name)
{
    // 打开文件
    FILE *fp;
    int fd;
    fp = fopen(elf_path, "r+");
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
    // 遍历
    uint8_t *btf_data;
    for (int i = 0; i < elf_head.e_shnum; i++) {
        temp = shstrtab;
        temp = temp + shdr[i].sh_name;

        if (strcmp(temp, ".BTF") != 0)
            continue;
        btf_data = (uint8_t *)malloc(sizeof(uint8_t) * shdr[i].sh_size);
        fseek(fp, shdr[i].sh_offset, SEEK_SET);
        long unsigned int read_size = fread(btf_data, sizeof(uint8_t) * shdr[i].sh_size, 1, fp);
        if (read_size <= 0) {
            printf("fread() failed!\n");
            exit(-1);
        }

        // 生成一个tmp文件，最后删除
        char tmp_fn[4096];
        snprintf(tmp_fn, sizeof(tmp_fn), "%s.btf", elf_path);
        fd = creat(tmp_fn, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            printf("open(%s) failed!\n", tmp_fn);
            exit(-1);
        }
        unsigned int write_size = write(fd, btf_data, sizeof(uint8_t) * shdr[i].sh_size);
        if (write_size != sizeof(uint8_t) * shdr[i].sh_size) {
            printf("%s: write of %ld bytes to '%s' failed\n",
                   __func__, sizeof(uint8_t) * shdr[i].sh_size, tmp_fn);
            unlink(tmp_fn);
        }

        if (!parseTypes(tmp_fn, func_name)) {
            exit(-1);
        }

        if (fd != -1)
            close(fd);
    }
    return 0;
}

bool parseTypes(const std::filesystem::path &filePath, const std::string &funcName)
{
    auto btf_res = btfparse::IBTF::createFromPath(filePath);
    if (btf_res.failed()) {
        std::cerr << "Failed to open the BTF file: " << btf_res.takeError() << "\n";
        return false;
    }
    auto btf = btf_res.takeValue();
    if (btf->count() == 0) {
        std::cout << "No types were found!\n";
        return false;
    }

    auto btf_type_map = btf->getAll();
    for (const auto &btf_type_map_p : btf_type_map) {
        const auto &id = btf_type_map_p.first;
        const auto &btf_type = btf_type_map_p.second;

        auto type_kind = btfparse::IBTF::getBTFTypeKind(btf_type);
        if (type_kind == btfparse::BTFKind::Func) {
            const auto &btf_func = std::get<btfparse::FuncBTFType>(btf_type);
            // if (btf_func.name.compare(funcName) == 0) {
            std::cout << std::to_string(id) << ": \t";
            std::cout << btf_func.name; //"\tfuncProto:\t" << btf_func.type << "\n";
            if (btf_func.name.compare(funcName) == 0) {
                std::cout << "\t Found!\n";
            } else {
                std::cout << "\n";
            }
            // 后面的内容为打印函数的参数
            // const auto btf_type_map_pp = btf_type_map.find(btf_func.type);
            // const auto &btf_funcproto = std::get<btfparse::FuncProtoBTFType>(btf_type_map_pp->second);
            // std::cout << "args" << "\n";
            // for (auto i : std::vector<btfparse::FuncProtoBTFType::Param>(btf_funcproto.param_list)) {
            //     if (i.opt_name.has_value()) {
            //         std::cout << i.type << "\t" << i.opt_name.value() << "\n";
            //     } else {
            //         std::cout << i.type << "\tunnamed" << "\n";
            //     }
            // }
            // }
        }
        else {
            continue;
        }
    }
    return true;
}

int getFuncBTF(const char *elf_path, const char *func_name)
{
    if (dumpAndParse(elf_path, func_name)) {
        return 0;
    }

    return -1;
}