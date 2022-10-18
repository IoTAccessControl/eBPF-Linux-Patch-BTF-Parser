Build deps时，会编译生成write_elf二进制!

## 测试步骤

```
bash gen_btf.sh xxx
```

1. 生成BTF符号到elf
用shell调用Pahole

2. 导出函数地址到elf
用shell调用Write_elf

## 功能
生成BTF section和 ~~sym副本~~