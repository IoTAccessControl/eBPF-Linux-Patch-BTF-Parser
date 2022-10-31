
## 基于不同的例子测试功能
本文件夹下包含5个不同的测试用例，分别为：自写简单可执行文件及动态库、openssl动态库、依赖openssl动态库的nginx可执行文件、apache可执行文件、memcached可执行文件。

&nbsp;
### step1：下载并编译相应程序
进入到xx_test文件夹下，运行如下命令以编译程序，
```bash
bash build.sh
```
注意：其中nginx在编译时需要使用openssl动态库，所以需要在build openssl完成之后再build nginx！

&nbsp;
### step2：为相应程序生成BTF信息，并比较文件大小变化
进入到xx_test文件夹下，运行如下命令，
```bash
bash 1-gen-btf.sh
```
可得到类似下面的输出，
```bash
### bash 1-gen_btf.sh
~/Documents/eBPF-Linux-Patch-BTF-Parser/test_prog/03-nginx_test ~/Documents/eBPF-Linux-Patch-BTF-Parser/test_prog/03-nginx_test
btf size increasing
file: /home/ubuntu/Documents/eBPF-Linux-Patch-BTF-Parser/test_prog/03-nginx_test/bin/nginx size: 1331144
file: /home/ubuntu/Documents/eBPF-Linux-Patch-BTF-Parser/test_prog/03-nginx_test/bin_btf/nginx size: 1446144
        increase: 1331144 -> 1446144 (inc: 115000 1299.95kb + 112.3kb 8.6%)
increase size after compressed
via tar
bin_btf/nginx.btf
        increase: 1331144 + 31217 (1299.95kb + 30.49kb 2.3%)
via zip
  adding: bin_btf/nginx.btf (deflated 73%)
        increase: 1331144 + 31232 (1299.95kb + 30.5kb 2.3%)
```
该脚本由多个命令组成：
1. pahole -J xxx：利用deps/dwarves-build/目录下的pahole工具，为带有debug信息的elf文件生成并添加BTF section
2. dump_btf xxx > xxx_btf.txt：利用btf_parser/目录下的dump_btf工具，解析BTF section，并将结果写入到txt文件中
3. strip xxx：利用strip命令删除elf文件中与debug信息相关的section，以及symtab和strtab
4. python check_size.py -c/-a xxx yyy：利用btf_writer/目录下的check_size.py，比较添加BTF信息后，文件的大小变化情况。其中“-c”选项，比较BTF section未压缩时的文件膨胀率，“-a”选项则比较利用tar或者zip压缩BTF section后的膨胀率

&nbsp;
### step3：验证BTF信息正确性
利用“nm -s”命令可以得到elf文件的符号信息，对其进行过滤后，可以和之前生成的BTF信息进行对比（这里对比的是函数名称），进入eBPF-Linux-Patch-BTF-Parser主目录，运行如下命令，
```bash
python btf_parser/check_btf_func.py xxx
```
执行结束后，会在当前文件夹生成func_diff文件。

&nbsp;
### 附：
01-simple_test和03-nginx_test文件夹中的2-parse-btf.sh脚本用来将BTF信息导出为json格式以及获取BTF信息中所有的函数的名称，其中使用到的工具bpftool来自Linux内核，链接为<https://github.com/libbpf/bpftool>；parse_btf位于btf_parser/build/目录下，利用了封装的C++库btfparse。