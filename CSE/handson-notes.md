# CSE hands-on notes

## [Hands-on 1  Domain Name Service](<https://ipads.se.sjtu.edu.cn/courses/cse/handson/handson-1.html>)

+ 提供一层域名到 IP 地址的映射，使人们能够使用更加友好也更加灵活的域名而不是难以记忆的 IP 地址。

+ record 有几种不同类型

  + `A` ：默认类型，查询域名的 IP 地址
  + `NS`：查询域名的权威 name server
  + `CNAME` ：查询域名的别名

+ `dig @8.8.8.8 www.baidu.com`

+ 先用该命令查询：

  ```bash
  dig lirone.csail.mit.edu +norecurs
  ```

  第一次查询只能得到 edu 的 name server： `a.edu-servers.net.` ，然后再向这个 name server 查询：

  ```bash
  dig @a.edu-servers.net. lirone.csail.mit.edu +norecurs
  ```

  就能得到 mit.edu 的 name server，如此反复就能解析出域名对应的 IP 地址。

+ twitter 解析出的 IP 地址每次都不一样而且 ping 不通，应该是墙的原因；加上 `@1.0.0.0` 就可以，可能是因为 1.0.0.0 是一个国外的 IP。

+ 解析出的 IP 每次都不一样，还是墙的问题。

## [Hands-on 2  Write Ahead Log System](<https://ipads.se.sjtu.edu.cn/courses/cse/handson/handson-2.html>)

+ 因为 action 2 没有 end

+ studentA 1000

+ 因为 action 3 的操作没有 end

+ studentC: 3100, studentA: 900，因为 action 3 commit 过，在 recover 的时候不会被 undo

+ yes

+ Losers 是指没有 commit，被 undo 掉的 action

  Winners 是指 commit 过，没有被 undo 的 action

  Done 是指原本就已经 end 过的 action

+ 因为 checkpoint 命令不会把数据 install 进 db

+ 6 lines，recover 的时候可以不用遍历整个 log

+ yes，幂等性

+ commit 过的 action 就不会被 undo

+ 会将没有 commit 的 action abort 掉

## [Hands-on 3  Load Balance and Reverse Proxy](<https://ipads.se.sjtu.edu.cn/courses/cse/handson/handson-3.html>)

+ 当负载不断增大时，吞吐量不再上升甚至会下降。

  ![](./handson-3/2.png)

+ 用 nginx 做负载均衡，减少 latency

  ![](./handson-3/4.png)

+ 提升 replica 数量，但是需要 redis 维护 session 信息，可能会提高 latency

  ![](./handson-3/5.png)

+ 限制 CPU 为 0.5，latency 提高了

  ![](./handson-3/6.png)

## [Hands-on 4  Name Service and Database Partitioning](<https://ipads.se.sjtu.edu.cn/courses/cse/handson/handson-4.html>)

+ 服务可以注册 watcher 监听 zookeeper server 中数据的变化，但是为了高可用，zookeerper server 往往不是单机，而是一个集群，集群中的机器保持一致性用的就是 paxos 协议。
+ 高可用，且集群中的机器可以保证一致性。
+ 1. 起一个 zookeeper server 在 10.0.0.5，并且创建一个用来保存 ip-hostname map 的 znode。
  2. 扒拉一段代码，将 Executor 类的构造函数入参改成自己 zookeeper server 的 ip，znode 的 name 以及 `/etc/hosts` （因为服务的容器中 ip-hostname map 存储在这个文件中）。在服务起来的时候，服务自身的 ip-hostname pair 会存储在 `/etc/hosts` 中，将其注册到 znode 上，并将 znode 中所有其他的 pair 拉取到自己的 `/etc/hosts` 里。
  3. 打包，修改 Dockerfile，起服务的时候不仅要起业务逻辑，还要额外起一个 zookeeper 的 jar 包。
+ 原本只起一个 Object-db 容器，现在起三个，因为在改 zookeeper 代码的时候加了一段 hostname 可以通过 DNS 环境变量导入的代码，所以三个 db 的 hostname 可以分别是 carts-db，user-db 以及 orders-db，这样不同的服务使用的就是不同的数据库。

##### Last-modified date: 2019.12.23, 12 p.m.

