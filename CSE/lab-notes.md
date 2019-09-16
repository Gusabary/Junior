# CSE lab notes

## Lab 1 Basic File System

+ `char *` 之间的赋值使用 `memcpy` 而不是 `*a = *b`

+ 以下两行代码效果差不多：

  ```c++
  char buf[SIZE];  // 不需要手动 free
  char *buf = (char *)malloc(SIZE);  // 需要手动 free
  ```

  未 malloc 的 `char *` 处于悬空状态，不能使用 `*buf` 获得指向的值。

+ 位运算 `&` 的优先级低于 `==` 

+ 有些情况下 segmentation fault 是由于某些函数在 corner case 的时候返回 null，然后 null 再指出去。

### 几个花了很长时间的坑

+ 程序中有几种 id，代表不同的意思（不同的 offset）：

  + 一个是相对于整个 disk 有一个 id，1 是 superblock，2 ~ 9 是 bitmap，12 ~ 1034 是 inode，后面是 data block。（好像是这样，但是我也不清楚 bitmap 和 inode 之间空着的是干嘛的）
  + 一个是 inum，是相对于 inode 段而言的，根目录就是 inum=1，id=12
  + 还有一个是 block id，是相对于 data block 段而言的，block id=0 就是 id=1035

  然后在 alloc_block 方法里面我没有把 id 和 block id 区分开，导致本来应该分配在 data block 段第 0 个 block 的我分配到了整个 disk 的第 0 个 block，然后分配到第 3 个的时候把 bitmap 给覆盖了。

+ 在 bitmap 中做位运算的时候我一开始是 `unsigned char` 和 `char` 在按位与，然后明明只有一个字节的 char 愣是打印出来 ffffff80，我猜想可能更符号位有关，然后我就把 `char` 改成 `unsigned char` ，但是因为接口定义的问题编译没过，我就把 `unsigned char` 改成 `char` 就能 work 了。看上去做位运算的两个操作数要同为 `unsigned` 或同不为 `unsigned` 。

+ 这个坑就纯属是我看代码没看仔细了，实现 symlink 的时候不仅要实现 symlink 和 readlink 方法，还要改一下 getattr 方法，因为 inode 的类型除了 file，dir，又加了一个 symlink。

p.s. 这 lab 我估摸着写了真有 24 个小时了。

##### Last-modified date: 2019.9.16, 4 p.m.

