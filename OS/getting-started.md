# Getting Started notes

## ARM

+ ARMv8 架构有两种 execution state：
  + AArch32，32 位通用寄存器，支持 A32 和 T32 指令集
  + AArch64，64 位通用寄存器，支持 A64 指令集
+ ARM 中一个 word 是 32 bits，4 字节。（x86 中一个 word 是 2 字节）

### 寄存器

+ AArch64 中的寄存器（寄存器名字中的 EL 表示 exception level）：
  + 31 个通用目的寄存器：X0 - X30（64 bit），其中这些寄存器的低位部分为 W0 - W30（32 bit）
  + 4 个 SP（stack pointer），栈顶指针，SP_EL0 - SP_EL3（64 bit）
  + 3 个 ELR（exception link register），保存返回地址，ELR_EL1 - ELR_EL3（64 bit）
  + 3 个 SPSR（saved program status register），在切换 EL 时保存程序状态，SPSR_EL1 - SPSR_EL3（32 bit）
  + 1 个 PC（program counter）
+ ARMv8 定义了 4 个 exception level，EL0 最低，EL3 最高：
  + EL0：Applications
  + EL1：Kernel
  + EL2：Hypervisor
  + EL3：Secure Monitor
+ X29 即为 FP
+ X30 即为 LR，和 ELR 不同的是，LR 保存的是 subroutine 的返回地址（在 exception level 内部调用并返回），而 ELR 保存的是 exception handler 的返回地址（有可能跨 exception level 调用并返回）
+ X31 有可能是零寄存器（ZR），也有可能是栈指针（SP）
+ condition flags:
  + N：negative
  + Z：zero / equal
  + C：carry
  + V：overflow
+ 栈顶指针的对齐为两倍指针长，对于 AArch64 来说即为 16 字节。

### 指令集

+ A64 指令集的一个指令为 32 bit 宽。

+ A64 中大部分指令有如下格式：

  ```assembly
  Instruction Rd, Rn, Operand2
  ```

  其中 Rd 是目的寄存器，Rn 是操作寄存器，Operand2 可以是寄存器或者立即数

+ Arithmetic：

  | 指令                              | 功能                                   |
  | --------------------------------- | -------------------------------------- |
  | add                               | 加                                     |
  | sub                               | 减                                     |
  | mul / mneg                        | 乘 / 乘完取反                          |
  | madd / msub                       | 乘加 / 乘减                            |
  | udiv / sdiv                       | 除（无符号 / 有符号）                  |
  | neg                               | 取负                                   |
  | adr / adrp （address（page））    | form PC-relative address               |
  | cmn / cmp （compare（negative）） | 比较（前者将两个操作数相加，后者相减） |

+ Logical and Move：

  | 指令                       | 功能                                                   |
  | -------------------------- | ------------------------------------------------------ |
  | and                        | 按位与                                                 |
  | orr / orn                  | 按位或 / 按位或非                                      |
  | mvn                        | 按位非                                                 |
  | eor / eon （exclusive or） | 按位异或 / 按位异或非                                  |
  | bic （bitwise bit clear）  | 先将第二个操作数取反，再按位与（按位置 0）             |
  | asr                        | 算数右移（复制符号位）                                 |
  | lsl / lsr                  | 逻辑左移 / 逻辑右移（补 0）                            |
  | mov                        | 赋值                                                   |
  | ror （rotate right）       | 旋转（循环右移）                                       |
  | tst                        | 和 and 类似，但是不保留结果，并且会改变 condition flag |

+ Load，Store and Addressing Modes

  | 指令      | 功能                      |
  | --------- | ------------------------- |
  | ldr / ldp | 读内存 / 一次写两个寄存器 |
  | str / stp | 写内存 / 一次读两个寄存器 |

+ Conditional：

  | 指令                  | 功能                                                         |
  | --------------------- | ------------------------------------------------------------ |
  | ccmn / ccmp           | 改变 condition flag，条件为真时用比较结果改变，为假时用第三个操作数改变 |
  | csel                  | 赋值，条件为真时，选择第二个操作数，为假时选择第三个操作数   |
  | csinc / csinv / csneg | 同 csel，但是条件为假时要先将第三个操作数 +1 / 取反 / 取负   |
  | cset / csetm（mask）  | 条件为真时将目的寄存器置为 1 / 全1，为假时置为 0 / 全0       |
  | cinc / cinv / cneg    | 赋值，条件为真时直接赋值，为假时先 +1 / 取反 / 取负          |

+ Bit Manipulation

+ Branch：

  | 指令       | 功能                                            |
  | ---------- | ----------------------------------------------- |
  | b / br     | 无条件 jump 到 label / 寄存器所指地址           |
  | b.cond     | 有条件 jump 到 label                            |
  | bl / blr   | call label / 寄存器所指地址                     |
  | ret        | return                                          |
  | cbz / cbnz | 如果寄存器值为 0 / 不为 0，就 jump 到 label     |
  | tbz / tbnz | 如果寄存器某一位为 0 / 不为 0，就 jump 到 label |

+ System：

  | 指令      | 功能                                         |
  | --------- | -------------------------------------------- |
  | msr / mrs | 将一个通用寄存器的值赋给系统寄存器 / 反过来  |
  | svc       | 产生 exception，陷入 EL1（类似 system call） |
  | nop       | 空指令，仅仅将 PC + 4，可用于指令对齐        |

## Makefile

+ > 代码变成可执行文件，叫做编译（compile）；先编译这个，还是先编译那个（即编译的安排），叫做构建（build）。

+ Make 即是最常用的构建工具之一。

+ Makefile 由一系列的规则组成，每条规则的格式如下：

  ```makefile
  <target> : <prerequisites> 
  [tab]  <commands>
  ```

  target 不可省略，prerequisites 和 commands 至少要有一个。

+ target 一般是文件名，指明 make 需要构建的对象。除此以外也可以是一个伪目标，即不是文件名，例如 `make clean`。直接运行 `make` 的话，默认会执行 Makefile 的第一条规则。

+ prerequisites 一般是一组文件名（空格分隔），指定目标是否需要重新构建的标准：只要有一个前置文件不存在或比目标更新（比较时间戳），就需要重新构建。（如果前置文件不存在需要先构建前置文件）

+ commands 表示如何构建，由一组 shell 命令构成。注意每行 shell 命令在单独的进程中执行，没有继承关系。

+ 回声：正常情况下，make 会打印每条命令（包括注释），在命令前加上@，可以关闭回声。但是由于在构建过程中，一般需要了解当前在执行哪条命令，所以通常只会关闭注释和纯显示命令（echo）的回声。

+ Makefile 定义了四种赋值运算符：

  + `=`：执行时扩展，lazy set
  + `:=`：定义时扩展，immediate set
  + `?=`：该变量为空时赋值，lazy set if absent
  + `+=`：append 到变量尾部

+ [*reference*](<http://www.ruanyifeng.com/blog/2015/02/make.html>)

## Linker Script

+ linker script，即链接器脚本，决定了可执行文件的链接方式，一般为 `.lds` 文件

+ 该文件主要由一个 `SECTIONS{}` 字段组成，其中包含每一段的信息，例如 VMA，LMA，段的内容等。

+ 每一段的格式大致是：

  ```
  secname vma : AT(lma) {
      contents
  }
  ```

  其中 vma 和 lma 都是可省的。

  contents 表示要将哪些目标文件的哪些段链接起来，可以使用通配符

+ `.lds` 文件中的 `.` 表示当前位置，可以被赋值成某个绝对地址，或者设置对齐（`ALIGN`）

+ 将 `.` 赋值给某个变量可以将当前地址保存下来，在程序中使用

+ *references*:

  + <https://blog.csdn.net/itxiebo/article/details/50937412>
  + <https://www.cnblogs.com/lotgu/p/5906161.html>

##### Last-modified date: 2020.3.11, 5 p.m.