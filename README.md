## NOTES 
1. 搭建开发环境，依赖于git, vim, clang, cmake, build-essential, g++, git, zlib1g-dev, llvm, python3, python3-pip, iproute2, libelf-dev等
```bash
bash scripts/build_all.sh
```

2. 为带有debug info的elf文件生成BTF section
```bash
bash btf_writer/gen_btf.sh file_with_debuginfo
```
![](test_prog/apache_test/apache_1.png)

3. 解析BTF信息，参数为elf路径和欲查找函数名（为方便测试，目前实现的是打印BTF中的所有函数名，若找到输入中的函数，则会额外打印Found）
```bash
./btf_parser/build/getBTF elf_path func_name
```
![](test_prog/apache_test/apache_3.png)

4. 比较nm -s输出的函数信息和btfparse库解析的是否相同
```bash
python btf_parser/check.py file_with_debuginfo
```
![](test_prog/apache_test/apache_4.png)