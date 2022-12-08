## patch生成

### 文件组成解释
- .c：带有漏洞的c文件
- .bpf.c：ebpf补丁文件
- .bpf.h：ebpf补丁头文件
- .patch.json：json格式补丁
- config.json：json格式补丁模板
- cJSON.c/cJSON.h：用于解析json补丁
- ebpf.h/ubpf_int.h/libubpf.a：ubpf运行时
- gen_patch.py：用于生成json格式的ebpf补丁
- Makefile：用于编译漏洞程序以及辅助生成json格式的补丁

### gen_patch.py用法
1. 相关函数
- compile_code()：使用clang编译ebpf补丁文件并提取补丁字节码
- getVulFuncAddress()：解析漏洞文件符号表，获取漏洞函数地址以及patch_handler()函数地址
- getVulPid()：获取正在运行的漏洞程序的pid
- fix_use_of_export_data()：生成一个会使用ebpf补丁中结构体的c文件，确保获取到struct信息，参考<https://github.com/eunomia-bpf/eunomia-cc>
- get_struct_memory_layout()：解析ebpf补丁中的结构体
- gen_json()：生成json格式的补丁

2. 使用方法

运行步骤：
- 单个测试
```shell
make bin/01-cve
python3 gen_patch.py -e bin/01-cve -f orig_c0 -s 01-cve.bpf.c -o bin/01-cve.patch.json
./bin/01-cve
```
- 整体测试
```shell
make test
```
或者
```shell
make main_test
```
(所有cve集中在一个文件，方便查看)

补丁示例：
```shell
{
    "name": "01-cve.bpf.c",
    "version": "0.0.1",
    "pid": 175579,
    "export_data_types": "",
    "address_vulFunc": "0x6766",
    "address_patchHandler": "0x68a1",
    "ebpf_patch": "\\x61\\x12\\x00\\x00\\x00\\x00\\x00\\x00\\x61\\x11\\x04\\x00\\x00\\x00\\x00\\x00\\x67\\x01\\x00\\x00\\x20\\x00\\x00\\x00\\x4f\\x21\\x00\\x00\\x00\\x00\\x00\\x00\\xb7\\x00\\x00\\x00\\x01\\x00\\x00\\x00\\x61\\x11\\x04\\x00\\x00\\x00\\x00\\x00\\x67\\x01\\x00\\x00\\x20\\x00\\x00\\x00\\xc7\\x01\\x00\\x00\\x20\\x00\\x00\\x00\\x65\\x01\\x02\\x00\\xb8\\x0b\\x00\\x00\\xb7\\x01\\x00\\x00\\x00\\x00\\x00\\x00\\x85\\x00\\x00\\x00\\x01\\x00\\x00\\x00\\x95\\x00\\x00\\x00\\x00\\x00\\x00\\x00",
    "ebpf_patch_len": 97,
    "pre_install_tests": [
        {
            "ebpf_test_code_1": "",
            "len": 0
        },
        {
            "ebpf_test_code_2": "",
            "len": 0
        }
    ],
    "description": "This is a template for a ebpf patch."
}
```