#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <fcntl.h>

#include <regex>
#include <cstring>
#include <sstream>
#include <iostream>
#include "btfparse/ibtf.h"
#include "wrapper.h"
#include "utils.h"

bool parseTypes(const std::filesystem::path &filePath, const std::string &funcName, const std::string &flag);

int dumpAndParse(const char *elf_path, const char *func_name, const char *flag)
{
    // 打开文件
    FILE *fp;
    int fd;
    fp = fopen(elf_path, "r+");
    if (fp == NULL)
    {
        printf("fail to open the file\n");
        exit(-1);
    }
    // 解析head,判断elf文件类型
    Elf64_Ehdr elf_head;
    int ret = fread(&elf_head, sizeof(Elf64_Ehdr), 1, fp);
    if (ret == 0)
    {
        printf("fail to read head\n");
        exit(-1);
    }

    if (elf_head.e_ident[0] != 0x7F || elf_head.e_ident[1] != 'E' ||
        elf_head.e_ident[2] != 'L' || elf_head.e_ident[3] != 'F')
    {
        printf("Not a ELF file\n");
        exit(-1);
    }
    // 解析section 分配内存 section * 数量
    Elf64_Shdr *shdr = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr) * elf_head.e_shnum);
    if (shdr == NULL)
    {
        printf("shdr malloc failed\n");
        exit(-1);
    }
    // 设置fp偏移量 offset
    ret = fseek(fp, elf_head.e_shoff, SEEK_SET); // fseek调整指针的位置，采用参考位置+偏移量
    if (ret != 0)
    {
        printf("faile to fseek\n");
        exit(-1);
    }
    // 读取section 到 shdr, 大小为shdr * 数量
    ret = fread(shdr, sizeof(Elf64_Shdr) * elf_head.e_shnum, 1, fp);
    if (ret == 0)
    {
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
    if (ret == 0)
    {
        printf("faile to read\n");
        exit(-1);
    }
    // 遍历
    uint8_t *btf_data;
    for (int i = 0; i < elf_head.e_shnum; i++)
    {
        temp = shstrtab;
        temp = temp + shdr[i].sh_name;

        if (strcmp(temp, ".BTF") != 0)
            continue;
        btf_data = (uint8_t *)malloc(sizeof(uint8_t) * shdr[i].sh_size);
        fseek(fp, shdr[i].sh_offset, SEEK_SET);
        long unsigned int read_size = fread(btf_data, sizeof(uint8_t) * shdr[i].sh_size, 1, fp);
        if (read_size <= 0)
        {
            printf("fread() failed!\n");
            exit(-1);
        }

        // 生成一个tmp文件，最后删除
        char tmp_fn[4096];
        snprintf(tmp_fn, sizeof(tmp_fn), "%s.btf", elf_path);
        fd = creat(tmp_fn, S_IRUSR | S_IWUSR);
        if (fd == -1)
        {
            printf("open(%s) failed!\n", tmp_fn);
            exit(-1);
        }
        unsigned int write_size = write(fd, btf_data, sizeof(uint8_t) * shdr[i].sh_size);
        if (write_size != sizeof(uint8_t) * shdr[i].sh_size)
        {
            printf("%s: write of %ld bytes to '%s' failed\n",
                   __func__, sizeof(uint8_t) * shdr[i].sh_size, tmp_fn);
            unlink(tmp_fn);
        }

        if (!parseTypes(tmp_fn, func_name, flag))
        {
            exit(-1);
        }

        if (fd != -1)
            close(fd);
    }
    return 0;
}

std::string getExactType(std::map<std::uint32_t, btfparse::BTFType> btfmap, uint32_t type)
{
    std::smatch results;
    std::regex pattern("(.*)type_id=(d*)(.*)");
    std::string xxx;
    std::stringstream ss;
    std::stringstream ss2;

    ss << btfmap.find(type)->second;

    do
    {
        std::getline(ss, xxx, '\n');
        if (std::regex_match(xxx, results, pattern))
        {
            // std::cout << "[" << results[3] << "] " << btfparse::IBTF::getBTFTypeKind(btfmap.find(std::stoi(results[3]))->second)
            //     << " " << btfmap.find(std::stoi(results[3]))->second << "\n";
            ss.clear();
            ss.str("");
            ss << btfmap.find(std::stoi(results[3]))->second;
            ss2.clear();
            ss2.str("");
            ss2 << btfparse::IBTF::getBTFTypeKind(btfmap.find(std::stoi(results[3]))->second);
        }
        else
        {
            // ss.clear();
            // ss << "struct or union";
            // switch
            break;
        }
    } while (1);

    int p;
    p = ss.str().find_first_of("'", 1);
    if (p > 0)
    {
        // return ss2.str() + "-" + ss.str().substr(1, p - 1);
        return (ss2.str().empty() ? ss.str().substr(1, p - 1) : ss2.str() + "-" + ss.str().substr(1, p - 1));
    }
    else
    {
        return "(anon)";
    }
}

void printStructOrUnion(std::map<std::uint32_t, btfparse::BTFType> btfmap, std::string soru, uint32_t type_soru)
{
    const auto btf_type_map_pp = btfmap.find(type_soru);
    const auto &type = std::get<btfparse::StructBTFType>(btf_type_map_pp->second);

    std::cout << "'"
              << (type.opt_name.has_value() ? type.opt_name.value() : "(anon)")
              << "' "
              << "size=" << type.size << " "
              << "vlen=" << type.member_list.size();

    if (!type.member_list.empty())
    {
        std::cout << "\n";
    }

    for (auto it = type.member_list.begin(); it != type.member_list.end(); ++it)
    {
        const auto &member = *it;

        std::cout << "\t"
                  << "'"
                  << (member.opt_name.has_value() ? member.opt_name.value() : "(anon)")
                  << "' "
                  << "type_str=" << getExactType(btfmap, member.type) << " " // CHANGE type_id to type_str
                  << "bits_offset=" << member.offset;

        if (member.opt_bitfield_size.has_value())
        {
            auto bitfield_size =
                static_cast<std::uint32_t>(member.opt_bitfield_size.value());

            if (bitfield_size != 0)
            {
                std::cout << " "
                          << "bitfield_size=" << bitfield_size;
            }
        }

        if (std::next(it, 1) != type.member_list.end())
        {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void printFunc(std::map<std::uint32_t, btfparse::BTFType> btfmap, uint32_t type)
{
    std::regex pattern("(.*)type_id=(d*)(.*)");

    std::cout << "[" << type << "] " << btfparse::IBTF::getBTFTypeKind(btfmap.find(type)->second)
              << " " << btfmap.find(type)->second << "\n";
    const auto btf_type_map_pp = btfmap.find(type);
    const auto &btf_funcproto = std::get<btfparse::FuncProtoBTFType>(btf_type_map_pp->second);

    // 1. get return type
    std::cout << "Return_type\n[" << btf_funcproto.return_type << "] " << btfparse::IBTF::getBTFTypeKind(btfmap.find(btf_funcproto.return_type)->second)
              << " " << btfmap.find(btf_funcproto.return_type)->second << "\n";

    std::smatch results_1;
    std::string xxx_1;
    std::stringstream ss_1;
    ss_1 << btfmap.find(btf_funcproto.return_type)->second;
    do
    {
        std::getline(ss_1, xxx_1, '\n');
        if (std::regex_match(xxx_1, results_1, pattern))
        {
            std::cout << "[" << results_1[3] << "] " << btfparse::IBTF::getBTFTypeKind(btfmap.find(std::stoi(results_1[3]))->second)
                      << " " << btfmap.find(std::stoi(results_1[3]))->second << "\n";
            ss_1.clear();
            ss_1.str("");
            ss_1 << btfmap.find(std::stoi(results_1[3]))->second;
        }
        else
        {
            // std::cout << ss.str() << "\n";
            // printStructOrUnion();
            break;
        }
    } while (1);

    // 2. get func parameters
    std::int32_t args_id = 1;
    std::int32_t tmp_type = 0;
    for (auto i : std::vector<btfparse::FuncProtoBTFType::Param>(btf_funcproto.param_list))
    {
        std::smatch results_2;
        std::string xxx_2;
        std::stringstream ss_2;
        std::cout << "Args_index: " << args_id++ << " " << (i.opt_name.has_value() ? i.opt_name.value() : "unnamed") << "\n";
        // std::cout << "[" << i.type << "] " << btfparse::IBTF::getBTFTypeKind(btfmap.find(i.type)->second)
        //         << " " << btfmap.find(i.type)->second << "\n";
        tmp_type = i.type;
        ss_2.clear();
        ss_2.str("");
        ss_2 << btfmap.find(i.type)->second;
        do
        {
            std::getline(ss_2, xxx_2, '\n');
            if (std::regex_match(xxx_2, results_2, pattern))
            {
                // std::cout << "[" << results_2[3] << "] " << btfparse::IBTF::getBTFTypeKind(btfmap.find(std::stoi(results_2[3]))->second)
                //     << " " << btfmap.find(std::stoi(results_2[3]))->second << "\n";
                tmp_type = std::stoi(results_2[3]);
                ss_2.clear();
                ss_2.str("");
                ss_2 << btfmap.find(std::stoi(results_2[3]))->second;
            }
            else
            {
                ss_2.clear();
                ss_2.str("");
                ss_2 << btfparse::IBTF::getBTFTypeKind(btfmap.find(tmp_type)->second);
                if (ss_2.str().compare("STRUCT") == 0 || ss_2.str().compare("UNION") == 0)
                {
                    printStructOrUnion(btfmap, ss_2.str(), tmp_type);
                    // std::cout << "test for struct or union\n";
                }
                else
                {
                    std::cout << "[" << tmp_type << "] " << btfparse::IBTF::getBTFTypeKind(btfmap.find(tmp_type)->second)
                              << " " << btfmap.find(tmp_type)->second << "\n";
                }
                break;
            }
        } while (1);
    }
    // needed ???
    // int num;
    // std::cout << "input the type_id you want to check: ";
    // std::cin >> num;
    // std::cout << getExactType(btfmap, num) << "\n";
}

bool parseTypes(const std::filesystem::path &filePath, const std::string &funcName, const std::string &flag)
{
    auto btf_res = btfparse::IBTF::createFromPath(filePath);
    if (btf_res.failed())
    {
        std::cerr << "Failed to open the BTF file: " << btf_res.takeError() << "\n";
        return false;
    }
    auto btf = btf_res.takeValue();
    if (btf->count() == 0)
    {
        std::cout << "No types were found!\n";
        return false;
    }

    auto btf_type_map = btf->getAll();
    for (const auto &btf_type_map_p : btf_type_map)
    {
        const auto &id = btf_type_map_p.first;
        const auto &btf_type = btf_type_map_p.second;

        auto type_kind = btfparse::IBTF::getBTFTypeKind(btf_type);
        if (type_kind == btfparse::BTFKind::Func)
        {
            const auto &btf_func = std::get<btfparse::FuncBTFType>(btf_type);
            if (flag.compare("all") == 0)
            {
                std::cout << "[" << std::to_string(id) << "]" << ": \t";
                std::cout << btf_func.name;
                if (btf_func.name.compare(funcName) == 0)
                {
                    std::cout << "\t Found!\n";
                }
                else
                {
                    std::cout << "\n";
                }
            }
            else
            {
                if (btf_func.name.compare(funcName) == 0)
                {
                    std::cout << "[" << id << "] " << btfparse::IBTF::getBTFTypeKind(btf_type)
                              << " " << btf_type << "\n";
                    printFunc(btf_type_map, btf_func.type);
                }
            }
        }
        else
        {
            continue;
        }
    }
    return true;
}

int getFuncBTF(const char *elf_path, const char *func_name, const char *flag)
{
    if (dumpAndParse(elf_path, func_name, flag))
    {
        return 0;
    }

    return -1;
}