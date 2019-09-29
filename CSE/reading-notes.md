# CSE reading notes

## Reading 1  Worse Is Better

+ worse is better 的哲学就是简单是最重要的，为了简单，正确性、一致性以及完整性都可以做出一些让步。

  比如某些公式，非常严谨非常精确，但是在工科的范畴中往往应用在实际问题上的是被简化，被近似后的公式。在误差允许的范围内牺牲一些正确性往往会极大地简化问题。

  再比如聊天工具中，很多时候一个人发出了消息，另一个人收到消息会有一定的延迟，但是可以将延迟对于发消息的人屏蔽，让他觉得消息已经发出去并且对方已经收到了，而不是一直显示正在发送的提示。这样牺牲一些一致性可以提升用户体验。

  再比如对于有些人来说，看历史书，宁愿选择只讲某朝某代某件具体的事的小说，也不愿意读涵盖上下五千年的通史，因为很多时候大而全会带来非常多的复杂性。

  所以正确、一致、完整，这些看上去都是非常棒的东西，但是为了简单，他们都可以做出让步。

+ 适用的场景：

  + 教学用的工具，比如 y86，Tigers
  + 一些机器学习的模型，使模型易于理解而牺牲一定的准确率

  不适用的场景：

  + 精密仪器制造，误差要求极其苛刻，不能为简单性做出让步
  + 银行系统的分布式存储，要求强一致性。

+ 暑假大作业的项目，我觉得是适用的，因为进度风险巨大，不得不放弃一些功能十分强大但学习成本很高的工具，而且一些 corner case 也没有办法考虑得尽善尽美。

## Reading 2  Fast File System

+ old file system 在用过一段时间以后性能下降很多，从每秒传送 175 KB 下降到每秒传送 30 KB。

+ 磁盘中数据的存储过于碎片化。

+ + Optimizing Storage Utilization：

    old file system 难以在 block 的大小上做权衡，将一个 block 分成多个 fragment 可以有效地解决存储大文件和小文件的问题。

  + File System Parameterization：

    old file system 忽略了底层硬件的参数，parameterization 可以针对不同的处理器能力和硬件特性给出最优配置。

  + Layout Policies：

    将相关的数据放在一起提高 locality 以减少寻道时间并实现大数据的传送。

+ + Optimizing Storage Utilization：

    当 free blocks 很少的时候该优化方法效率会下降。

  + File System Parameterization：

    当硬件移动到另一个参数化的文件系统中时，有可能会出现参数不匹配的状况，会导致吞吐量急剧下降。

  + Layout Policies：

    对于维持较多 free space 的文件系统来说，该优化用到的哈希算法不具备性能优势。

+ 缓存需要频繁读写的 inode table。

##### Last-modified date: 2019.9.29, 11 a.m.