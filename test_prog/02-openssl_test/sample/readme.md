动态链接libcrypto.so测试
1. 使用make编译出hash_file,在运行之前可能需要先修改环境变量，如
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

3. nginx编译使用本地openssl
```bash
cd openssl-3.0.5
./config -d --prefix=/usr/local/openssl3
make
```
```bash
cd nginx-1.20.0
./configure --prefix=/usr/local/nginx --with-http_ssl_module --with-ld-opt="-L../openssl-3.0.5"
make CFLAGS="-g -O0"
```
```bash
patchelf  --set-rpath path_of_openssl-3.0.5 objs/nginx
```
结果
```bash
$ ldd objs/nginx                                            
	linux-vdso.so.1 (0x00007ffcfabe6000)
	libcrypt.so.1 => /lib/x86_64-linux-gnu/libcrypt.so.1 (0x00007f8d2e39f000)
	libpcre.so.3 => /lib/x86_64-linux-gnu/libpcre.so.3 (0x00007f8d2e329000)
	libssl.so.3 => /home/ubuntu/Pictures/openssl-3.0.5/libssl.so.3 (0x00007f8d2e252000)
	libcrypto.so.3 => /home/ubuntu/Pictures/openssl-3.0.5/libcrypto.so.3 (0x00007f8d2dd3e000)
	libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f8d2dd22000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8d2daf8000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f8d2e7b0000)
```