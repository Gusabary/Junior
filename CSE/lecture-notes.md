# CSE lecture notes

## Lecture 1  Introduction - System Complexity

+ system complexity
  + emergent properties (surprise)
  + propagation of effects (butterfly effect)
  + incommensurate scaling 
  + trade-offs
+ coping with complexity -- **MALH**
  + **M**odularity
  + **A**bstraction
  + **L**ayering
  + **H**ierarchy

## Lecture 2  Inode-based File System

Seven layers abstraction

![](./images/fs1.png)

### Block layer

block number -> block data

+ One superblock per file system

### File layer

inode -> block numbers

inode + offset -> block number

### Inode number layer

inode number -> inode

+ inode number is the *index* of inode table

  ![](./images/fs2.png)

### File name layer

dir + filename -> inode number

+ filename in current dir
+ Mapping table is saved in directory
  + This means if an inode serves as a **directory**, its blocks store a **table** mapping filename and inode number, and if it serves as a **file** instead, its blocks store pure **data**.

### Path name layer

dir + path -> inode number

+ path starts from current dir
+ path can be deeper than filename
+ Users cannot create links to directories, (avoiding a cycle) but except for `.` and `..`

### Absolute path name layer

path -> inode number

- path can be absolute or relative
- root inode number is **1**
- Both `/.` and `/..` are linked to `/`

### Symbolic link layer

## Lecture 3  File System API

## Lecture 4  Disk I/O

+ polling -> interrupts -> hybrid
+ DMA
+ PIO / Memory-mapped I/O

## Lecture 5  File System Design

+ FFS  Cylinder group
+ New Storage and Media

## Lecture 6  Crash Consistency

### MTTF & Availability

+ MTBF = MTTF + MTTR
+ MTTF 和 Availability 是相互正交的两个概念，可以 MTTF 很大而 Availability 很小，也可以 MTTF 很小而 Availability 很大。

### Redundancy

+ Coding - Hamming Distance
+ Replication
+ Voting - NMR (N-modular redundancy)
+ Repair

### Magnetic disk fault tolerance

+ RAID

### FSCK

### Logging / Journaling

+ D -> Jm -> Jc -> M，在 commit 之前，新 metadata 不会覆盖旧 metadata，可以很方便地回滚。
+ 不 flush -> probabilistic crash consistency，但是在某些 workload 下会有问题，有两种方案：
  + checksum，可以去掉第一个 checksum
  + delayed writes，可以去掉第二个 checksum

## Lecture 7  RPC & NFS

### RPC

+ stub
+ UDP header
+ marshal / unmarshal

### NFS

+ file handler
  + file system identifier
  + inode number
  + generation number
+ stateless on NFS server

## Lecture 8  GFS

+ Master does not use any per-directory data structures
+ Contains chunks (blocks, each 64MB) of a fixed size

### read

<div>
    <img src="images/gfs-read.png" width="45%" />
</div>

### write

<div>
    <img src="images/gfs-write1.png" width="45%" />
    <img src="images/gfs-write2.png" width="45%" />
</div>

## Lecture 9  CDN & DNS

## Lecture 10  Introduction to Network and Link Layer

![](./images/network1.png)

![](./images/network2.png)

### Application Layer

+ Entities: client and server
+ Namespace: URL

### Transport Layer

+ Entities: sender and receiver

+ Namespace: port

+ TCP: Retransmit packet if lost

  UDP: Nothing

  ![](./images/tcp-udp.jpg)

### Network Layer

+ Entities: gateway, bridge, router...
+ Namespace: IP address
+ What to care: next hop decided by route table

![](./images/ip.png)

### Link Layer

+ Entities: hub, switcher, twisted line, cable line...
+ Namespace: no name needed
+ What to care: physical transfer, error detection

### Link Layer Detail

#### Physical transmission

- parallel transmission
- serial transmission
- VCO
- Manchester Code 0 -> 01, 1 -> 10

#### Multiplexing the link

- Isochronous communication
- Asynchronous communication

#### Framing bits & bit sequence

+ Choose a pattern of bits, e.g., 7 one-bits in a row, as a frame-separator (where a frame
  begins and ends)
+ Bit stuffing: if data contains 6 ones in a row, then add an extra bit 0

#### Detecting transmission errors

+ fault tolerancy -> redundancy
  + simple parity check (2 bits -> 3 bits)
  + 4 bits -> 7 bits

#### Providing a useful interface to the up layer

## Lecture 11  Network Layer

+ IP: Best-effort Network

![](./images/network-layer.png)

### NAT

+ Network Address Translation

  ![](./images/nat.png)

## Lecture 12  Network Layer: Routing

+ 为了实现冲突检测，包的最小长度要满足接收这个包的时间大于包传递的最大延迟，这样即可保证在一个包发出和接收的两个时刻之间没有其他包被发出。
+ 路由器和交换机的区别：
  + 路由器：网络层，IP 地址，不同网段
  + 交换机：链路层，MAC 地址，同一网段
+ IP 地址和 MAC 地址的区别：IP 地址本质上是终点地址，它在跳过路由器（hop）的时候不会改变（NAT 除外），而 MAC 地址则是下一跳的地址，每跳过一次路由器都会改变。

### ARP

+ Address Resolution Protocal
+ Name mapping: IP address <-> MAC address
+ ARP Spoofing / Man-in-the-Middle Attack: 污染 ARP cache，使得包被发往错误的 MAC 地址。

### Routing

构建路由表的两种方法：

+ Link-state Routing： 告诉所有节点自己到邻居的距离（只需要告诉一次就能在每个节点构建出网络拓扑）
+ Distance-vector Routing： 告诉邻居节点自己到所有节点的当前距离（当前距离是指可能会经过多次更新，路由表才会到达最优状态）

#### Problem of Infinity

由于 Distance-vector Routing 方法中不同节点的传播顺序不同，有可能会造成明明两个节点之间的网络已经崩了，但是还能通过另一个节点的数据虚假地恢复出来，而且和不可通信的节点之间的距离会不停增大。

Split Horizon 是一个尝试性的解决方案，即如果路由表中的某条记录是某个节点告诉我的，那我就不再把这条记录告诉那个节点，但仍然不能彻底解决这个问题。

#### Scale to Internet

+ Path Vector Exchange：不仅告诉邻居自己到所有节点的当前距离，还告诉他们这些距离是经过哪些节点走出来的。
+ Hierarchical Address Assignment：引入层级结构，简化路由表。
+ Topological Addressing：进一步简化路由表，CIDR Notation，子网掩码。

## Lecture 13  End-to-end Layer

### BGP

+ Border Gateway Protocal
+ Customer / Provider / Peer

### End-to-end layer

+ No "one size fits all": UDP / TCP / RTP

#### Assurance of at-least-once delivery

Remember state at the **sending side**

RTT (Round-trip time) = to_time + process_time + back_time (ack)

How to decide timeout ?

+ Fixed timer: Evil
+ Adaptive timer
+ NAK (Negative ACK)

#### Assurance of at-most-once delivery

Maintains a table of nonce at the **receiving side**

Tombstones

#### Assurance of data integrity

data integrity: Receiver gets the same contents as sender

Checksum

#### Assurance of stream order & closing of connections

when out of order: receiving side window

#### Assurance of jitter control

#### Assurance of authenticity and privacy

#### Assurance of end-to-end performance

+ Lock-step

+ Pipeline

  + Fixed window

  + Sliding window

    window size = round-trip time * bottleneck data rate

### TGP Congestion Control

Network & End-to-end layers share the responsibility for handling congestion

#### AIMD

+ Additive Increase, Multiplicative Decrease

+ retrofitting: slow start

+ AIMD leads to efficiency and fairness

  ![](./images/AIMD.png)

##### Last-modified date: 2019.10.30, 11 a.m.

