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

## Lab 3 Cache for Locks

这个 lab 设计阶段还是挺有意思的，麻烦就麻烦在不知道从哪里冒出来的竞争，需要很仔细很仔细地看代码。

我的设计是 client 和 server 各维护一个 map，client 的 map 是 lock id 到 lock status 状态的映射，server 的 map 是 lock id 到正在排队的 clients 的队列的映射。

当有线程想要拿锁时，会给 client 发 acquire，如果 client 没有锁就去向 server 发 acquire 并将自己排在队列尾部，server 会给他前面的那个人（队列的倒数第二个）发 revoke，client 收到 revoke 后会先看自己这里的锁还有没有人在用，如果没人用就直接归还给 server，如果还有人在用就先记录下 revoke has arrived 这件事情，等用完了再放锁，server 拿到 client 放的锁后会给之前发 acquire 请求的 client 发一个 retry，把锁给他。

当有线程放锁时，会给 client 发 release，client 先查看是否有待处理的 revoke 请求，如果有，立刻将锁归还给 server，如果没有就在自己这放着，等 server 发 revoke，或者有新的线程来申请锁。

有几点需要注意：

+ `pthread_cond_signal` 会唤醒由于 `pthread_cond_wait` 而睡过去的线程，但是不会立刻执行，即控制流不一定会转移到被唤醒的线程。
+ 在发 rpc 的时候不要拿着锁，一个解决方法是：进入函数立刻拿锁，发 rpc 之前放锁，rpc 返回立刻拿锁，return 前放锁；在这种情况下需要特别小心，因为一个原本被锁保护的函数的原子性被 rpc 拦腰截断了，有可能发 rpc 发着发着去执行别的线程了，执行了很久才回来继续执行这个函数。
+ `pthread_cond_wait(&(lock_manager.find(lid)->second), &mtx);` 这种写法会在睡觉前放锁，醒来后拿锁（且这些操作是具有原子性的）
+ `yfs_client` 里面没替换成 `lock_client_cache` 前，`./start.sh` 再 `ls` 没反应。
+ `RPC_LOSSY` 的意思是丢包率，但是由于 TCP 协议的存在，丢包的情况并不需要手动重发，只是要考虑包的顺序可能和预期的不一致。（但是一般 `RPC_LOSSY=0` 能过 `RPC_LOSSY=5` 也就能过了

> 多线程调试有几个重要的方法，仔细地阅读自己的代码，多打印些信息。 —— Veiasai

## Lab 4 Cache for Data

这个 lab 也还算有意思，一开始我感觉挺简单的，但是写到后面还是有不少细节。

最开始我的设计是第一次 get 去向 server 拿，然后之后的每一次 get 拿的都是 cache 里的数据，但是对于每一次 write，都要写穿到 server 并且 server 要通知所有缓存了这一项数据的 client 更新他们的 cache。这乍一听没什么问题，get 数量远远多于 write，优化 get 的收益更大，但是效果是 rpc 的数量只降到了 2000+ （原来 11000+），虽然还是挺明显的，但是离十分之一的要求还有些距离。

后来我发现了 yfs_client 中给 extent_client 的请求只有固定的几种模式，比如 create->get->put 这种情况，我是 create 和 put 都发了 rpc，但是完全可以把他们合并成一次，经过一番操作以后，确实降到了十分之一。

有几点需要注意：

+ yfs_client 持有 extent_client 和 lock_client 两个需要接受 rpc 的实例，为他们分配端口时设置随机种子要不一样。
+ 如果引入了新文件 extent_client_cache, extent_server_cache，要把它们添加到 GNUMakefile 中。
+ rpc 一次最好不要发送太多数据。

##### Last-modified date: 2019.11.17, 10 p.m.

