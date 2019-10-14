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

## Lab 2 RPC and Lock Server

+ 这个 lab 需要有一个 server，一个 client，一开始我跑了两次 `docker run` ，结果 client 怎么也访问不到 server（通过 localhost），因为它们压根就不在同一个容器，有这么几种解决方案：
  + 先用 `docker run` 起一个容器，在容器中起一个 server，然后在另一个终端用 `docker exec` 进入这个容器，再起一个 client，就可以访问到 server。
  + 或者不用开另一个终端也可以，用 `./demo_server ${port} &` 这个命令，即在要执行的命令后面加一个 `&`，就可以让它在后台执行。
  + 再或者，把它们起在两个容器中，然后用自定义的 network bridge 或者 `--link` 把它们连通，这种方法我没试过不过应该可以，因为可以指定 client 访问的 ip。
+ lock_server.h 的类中声明的 protected 变量应该是线程共享的，可以在这里定义 mutex，condition variable map。
+ 在 yfs_client.cc 里面，由于我的实现中 create 调用了 lookup，所以这两个函数都要上锁的话会导致死锁，然后我看了一下因为 lookup 只会被 yfs_client.cc 里面的函数调用，就直接把 lookup 里面申请锁和释放锁的操作注释掉了。

### 大坑

在做 part2 的时候，测试时过时不过，很烦，问题表现为测试脚本会卡住。

一开始我以为是死锁的问题，后来发现不是，问题真正出在 lab1 部分的 write file 函数。在这个函数里面要向一个 inode 管着的 blocks 里面写东西，写一个字符串。我一开始的处理方式是把这个字符串按照 BLOCK_SIZE 分段，一段一段地写到对应的 block 里面，但是问题是字符串的最后一段很有可能长度不足一个 BLOCK_SIZE，但是我没有做处理，而是不加分别地写进去 BLOCK_SIZE 个字符，导致 memcpy 卡住（虽然我也不知道为什么超过字符串长度会让 memcpy 卡住，但是几乎可以肯定问题就出在这里）。

找到问题之后，对字符串的最后一段做一下特殊处理就可以了。

btw，调试阶段起容器时可以用 `-m 300M` 这样的选项限制容器可使用的内存大小，避免机子垮掉。

##### Last-modified date: 2019.10.13, 10 p.m.

