动态链接libcrypto.so测试
1. 使用make编译出hash_file,在运行之前需要先修改环境变量，如
```bash
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ubuntu/Documents/eBPF-Linux-Patch-BTF-Parser/test_prog/02-openssl_test/openssl-1.1.1q/
export LD_LIBRARY_PATH
```
之后再运行

2. ldd结果
```bash
sample (master) ✗ ldd hash_file
	linux-vdso.so.1 (0x00007ffceb1f3000)
	libcrypto.so.1.1 => /home/ubuntu/Documents/eBPF-Linux-Patch-BTF-Parser/test_prog/02-openssl_test/openssl-1.1.1q/libcrypto.so.1.1 (0x00007f047caf9000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f047c8c1000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f047ce54000)
 sample (master) ✗
```

3. nginx编译使用本地openssl，网上查到的方法是--with-openssl=/usr/local/openssl选项，但是没成功