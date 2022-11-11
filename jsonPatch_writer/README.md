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
```shell
make all
python3 gen_patch.py -e 01-cve -f orig_c0 -s 01-cve.bpf.c -o 01-cve.patch.json
./01-cve
```

补丁示例：
```shell
{
    "name": "01-cve.bpf.c",
    "version": "0.0.1",
    "pid": 18482,
    "export_data_types": [
        {
            "Fields": [
                {
                    "Name": "a1",
                    "Type": "int",
                    "LLVMType": "i32",
                    "FieldOffset": 0
                }
            ],
            "Struct Name": "Stack_frame",
            "Size": 32,
            "DataSize": 32,
            "Alignment": 32
        }
    ],
    "address_vulFunc": "0x1539",
    "address_patchHandler": "0x166e",
    "ebpf_patch": "\\xb7\\x00\\x00\\x00\\x01\\x00\\x00\\x00\\x61\\x11\\x00\\x00\\x00\\x00\\x00\\x00\\x67\\x01\\x00\\x00\\x20\\x00\\x00\\x00\\xc7\\x01\\x00\\x00\\x20\\x00\\x00\\x00\\x65\\x01\\x02\\x00\\x88\\x13\\x00\\x00\\xb7\\x01\\x00\\x00\\x64\\x00\\x00\\x00\\x85\\x00\\x00\\x00\\x01\\x00\\x00\\x00\\x95\\x00\\x00\\x00\\x00\\x00\\x00\\x00",
    "ebpf_patch_len": 65,
    "description": "This is a template for a ebpf patch."
}
```