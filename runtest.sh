#/bin/bash
riscv64-linux-gnu-objcopy -S -O binary xv6-user/_init oo 
od -v -t x1 -An oo | sed -E 's/ (.{2})/0x\1,/g' > kernel/include/initcode.h 
rm oo 
