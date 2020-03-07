# OS lab notes

## Lab 1  Booting a machine

+ printf 只有 10 进制可以为负

### Exercises

1. 阅读文档

2. ```bash
   (gdb) where
   #0  0x0000000000080000 in _start ()
   ```

   `_start()` 函数定义在 `start.S` 文件中

3. 对于 secondary processors，`start.S` 中的处理是用一个无限递归给他 hang 住：

   ```assembly
   secondary_hang:
   	bl secondary_hang
   ```

   一开始我在想为什么用 `bl` 而不用 `b`，无限递归的话栈不会越长越大吗，后来一想可能是因为 AArch64 中返回地址是保存在寄存器中的（但是似乎用 `b` 也不是不可以啊）

4. pointer warmup

5. `init` 段的代码 VMA 和 LMA 相同，而 `.text` 段的代码 VMA 和 LMA 不同：

   ```
   Name     Size      VMA               LMA             
   init     00049680  0000000000080000  0000000000080000
   .text    00000760  ffffff00000cc000  00000000000cc000
   ```

   经过一番学习，了解到 LMA 为代码在内存中存储（加载）的地址，而 VMA 为代码运行时的地址，它们是由链接器脚本决定的，也就是 `.lds` 文件：

   ```
   . = TEXT_OFFSET;
   init : {
   	${init_object}
   }
   
   . = ALIGN(SZ_16K);
   init_end = ABSOLUTE(.);
   
   .text KERNEL_VADDR + init_end : AT(init_end) {
   	*(.text*)
   	. = ALIGN(SZ_16);
   }
   ```

   这是该脚本中的一段代码，有几点可以解释上面 `objdump` 出来的信息：

   + `.` 表示当前位置，第一行将其赋值为 `TEX_OFFSET`，而 `TEXT_OFFSET` 宏在 `image.h` 中被定义为 `0x80000`，所以 `init` 段的地址为 `0x80000`，且默认 VMA 和 LMA 相同。
   + `init_object` 在 `CMakeLists.txt` 中被设置为 boot loader 相关的目标文件，所以 `init` 段中存储的是 boot loader 相关的代码
   + `init` 段起始地址为 `0x80000`，大小为 `0x49680`，再加上 `SZ_16K`（在 `image.h` 文件中被定义为 `0x4000`）的对齐，可以得到 `init_end` 的值恰好为 `0xcc000`
   + `.lds` 文件的语法规定了 section name 后可选地跟一个 VMA，然后是冒号，再然后是 AT(LMA)，经过计算 `.text` 段的 VMA 和 LMA 恰如以上信息中所示
   + `.text` 段的内容使用通配符匹配所有目标文件的 `.text` 段，在 `CMakeLists.txt` 中 `kernel` 目录被 include 了进来，所以 `.text` 段中存储的是 kernel 相关的代码。
   + kernel 相关的代码在运行的时候存储在高地址，所以 VMA 会加上一个 offset，也即 `KERNEL_VADDR`

6. 实现 print 函数

7. 

 