### 项目Build  
1. 安装依赖  
```
sudo apt install git vim clang cmake build-essential g++ git zlib1g-dev llvm python3 python3-pip iproute2 libelf-dev \
	 libdw-dev libpcre3-dev libexpat1-dev 
```

2. build  
```
bash scripts/build_all.sh  
```

3. 开发测试BTF功能  
修改pahole代码，  
```
# 单独编译deps，修改pahole之后需要单独编译  
bash scripts/build_pahole_patch.sh  

# 测试pahole
deps/dwarves-build/pahole -J test_prog/03-nginx_test/bin_btf/nginx


# global变量实现
deps/dwarves-build/pahole -J test_prog/01-simple_test/bin/libtest.so
btf_parser/dump_btf test_prog/01-simple_test/bin_btf/libtest.so > func_btf.txt

# 查看drawf信息
readelf -wi test_prog/01-simple_test/bin/libtest.so
```


#### 用simple作为例子，验证BTF正功能  
1. 确认BTF内容是否正确
```
bpftool -j btf dump file test_prog/01-simple_test/bin/simple
```


2. 验证BTF大小  
[测试步骤：](test_prog/README.md)
```
# 依次执行以下命令，可查看不同项目的压缩比例
bash test_prog/01-simple_test/1-gen_btf.sh
bash test_prog/01-simple_test/2-parse_btf.sh 
bash test_prog/02-openssl_test/1-gen_btf.sh
bash test_prog/03-nginx_test/1-gen_btf.sh
bash test_prog/03-nginx_test/2-parse_btf.sh
bash test_prog/04-apache_test/1-gen_btf.sh
bash test_prog/05-memcached_test/1-gen_btf.sh
```

#### 验证Patch功能  
