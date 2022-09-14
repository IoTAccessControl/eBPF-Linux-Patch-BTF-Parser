

## 任务  
1. pahole_patch 生成git diff文件    √
2. 写build shell脚本(build_pahole函数写完)  √
3. 写完利用pahole生成btf的全过程：  √
>> btf_writer\gen_btf.sh  
4. 测试上面btf功能  
- 利用test_prog里面的库来看btf对不对    √
- 用系统的openssl来测试功能     √
5. 尝试dump 函数符号到elf中     √
6. 封装bpfparser库，到btf_parser例子    √
    - git submodule add https://github.com/trailofbits/btfparse
    - cd btfparse
    - git submodule update --init
    - cd ..
    - mkdir build && cd build
    - cmake .. && make
