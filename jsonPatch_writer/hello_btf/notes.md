## 搞清楚BTF的relocate

### 确认BTF信息
搞清楚bpf elf中的BTF信息包含哪些字段，是如何relocate的
```
apt install binutils-bpf

### 编译。dump section
clang -g -O2 -target bpf -D__TARGET_ARCH_x86_64 -I . -c 04-cve.bpf.c -o hello.bpf.o
bpf-readelf -a hello.bpf.o

### 获取bpf的asm
bpf-objdump -S hello.bpf.o > asm.list
llvm-objdump -S --no-show-raw-insn hello.bpf.o > raw_asm.list

### 查看bpf.ext字段
The .BTF.ext section encodes func_info and line_info which needs loader manipulation before loading into the kernel.

```
insn #4 --> 18:6 04-cve.bpf.c
		if (lh == NULL)
  20:	15 01 0d 00 00 00 00 00 	jeq %r1,0,13

insn #6 --> 21:2 04-cve.bpf.c
		for (i = 0; i < lh->num_nodes; i++)


insn #10 --> 23:11 04-cve.bpf.c
			n = lh->b[i];
50:	79 10 00 00 00 00 00 00 	ldxdw %r0,[%r1+0]
			r0 = *(u64 *)(r1 + 0)
```
bpf是如何获取一条指令对应的变量类型：
每一条指令能对应到源码