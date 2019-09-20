# 编译原理学习笔记

## 第 1 章 绪论

+ **直线式程序** (straight-line program) 语言：有语句和表达式，但是没有循环和 if 语句，比如：

  ```
  Stm -> id := Exp
  ```

+ 表达式序列 `(s,e)` 的行为类似于 C 语言中的逗号操作符，在计算表达式 `e` 之前先计算语句 `s` 的**副作用**。所以 `s` 中的赋值操作有可能会影响 `e` 的结果。

### Lab 1: Straight-line Program Interpreter

+ `.h` 文件中最好不要写函数的实现，因为实现的代码有可能会调用别的类的方法，而在头文件中只有别的类的方法的定义而没有实现，会报错。

  因为是在 `.cpp` 文件中引用 `.h` 文件，编译时刻 `.h` 文件找不到 `.cpp` 文件。

+ `std::cout` 需要引入 `<iostream>` 库。

+ 报错 `'nullptr' was not declared in this scope` 有可能是 CMake 版本过低。

+ 在以 `a->f(x)` 这样的方式调用函数并返回 `x` 时，需要小心 `x` 是否改变，如果没变，极有可能应该写成

   `new_x=a->f(x)` ，然后返回新的 `new_x`。

+ 如果有这样的写法：

  ```c++
  a = f()->a;
  b = f()->b;
  ```

  最好能写成：

  ```c++
  sometype *tmp = f();
  a = tmp->a;
  b = tmp->b;
  ```

  避免执行两次 `f` 带来预料之外的错误。

## 第 2 章 词法分析

+ 两条重要的消除二义性的规则：最长匹配，规则优先。

### 正则表达式 => NFA

五种基本转换：

+ for ε
+ for input a
+ for AB
+ for A | B
+ for A*

### NFA => DFA

+ 先确定 DFA 的 start state：NFA 的 start state + 从该 start state 出发经过 ε 可到达的 state。
+ 在确定 DFA 的其他 state：从 DFA 的 start state 群中任一 state 出发，经过某 transition 和任意数量的 ε 可到达的 state。

### 用 ㅣ，ㅜ，ㅗ 做词法分析

+ ㅣ 表示当前 lexeme 的起始位置
+ ㅜ 表示遇到的最后一个 final state 的位置
+ ㅗ 表示当前扫描到的位置

开始新一轮扫描后，ㅣ不变，ㅜ，ㅗ 往前扫描，遇到 final state，ㅜ 停下，ㅗ 继续往前扫描，再遇到 final state，更新 ㅜ 位置，ㅗ 继续往前扫描，直到在状态机图中走不通为止，将ㅣ，ㅗ 都置于 ㅜ 所在的位置。

### Lab 2: Lexical Analysis

+ ctrl + A （^A）对应的 ascii 码是 1，^B 是 2，以此类推。
+ atoi 的参数的第一个字符似乎要求是数字或者正负号，不然会转换失败，至少第一个字符是 `\` 不行。
+ `"{digit}"` 匹配由这七个字符本身构成的字符串，`{digit}` 匹配 0 ~ 9 （如果前面定义了的话）

##### Last-modified date: 2019.9.20, 10 p.m.