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
1. 
``` bash
via tar
bin_btf/libtest.so.btf
        increase: 14032 + 255 (13.7kb + 0.25kb 1.8%)
via zip
  adding: bin_btf/libtest.so.btf (deflated 49%)
        increase: 14032 + 314 (13.7kb + 0.31kb 2.2%)
```