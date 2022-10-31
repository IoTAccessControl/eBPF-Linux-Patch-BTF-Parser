### 项目Build  
1. 安装依赖  
```
sudo apt install git vim clang cmake build-essential g++ git zlib1g-dev llvm python3 python3-pip iproute2 libelf-dev \
	 libdw-dev libpcre3-dev
```

2. build  
```
bash scripts/build_all.sh  
# 单独编译deps，修改pahole之后需要单独编译  
bash scripts/build_pahole_patch.sh  
```

#### 用simple作为例子，验证正确性  
1. 确认BTF内容是否正确
```
bpftool -j btf dump file test_prog/01-simple_test/bin/simple
```


#### 验证BTF大小  
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