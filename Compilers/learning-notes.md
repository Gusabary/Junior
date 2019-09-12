# 编译原理学习笔记

## 第一章 绪论

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

##### Last-modified date: 2019.9.12, 9 p.m.