#include "btf_tool.h"
#include "btfparse/ibtf.h"

using namespace std;

bool parseTypes()
{
    static const std::vector<std::filesystem::path> kPathList{
        "/home/ubuntu/Music/ProcessInjectionDemo/BTFParse/tmp"}; // /sys/kernel/btf/vmlinux
    auto btf_res = btfparse::IBTF::createFromPathList(kPathList);
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

    for (const auto &btf_type_map_p : btf->getAll())
    {
        const auto &id = btf_type_map_p.first;
        const auto &btf_type = btf_type_map_p.second;

        auto type_kind = btfparse::IBTF::getBTFTypeKind(btf_type);
        if (type_kind != btfparse::BTFKind::Struct)
        {
            continue;
        }
        const auto &btf_struct = std::get<btfparse::StructBTFType>(btf_type);

        std::cout << std::to_string(id) << ": ";
        if (btf_struct.opt_name.has_value())
        {
            std::cout << btf_struct.opt_name.value() << "\n";
        }
        else
        {
            std::cout << "unnamed\n";
        }
    }
    return true;
}
// int hook_test(struct A *t)
// {
// 	return t->a + 22;
// }

int main()
{
    struct A tmp_1 = {'a', 11};
    int tmp_2 = test(&tmp_1);
    printf("num: %d\n", tmp_2);

    parseTypes();

    return 0;
}