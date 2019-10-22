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

##### Last-modified date: 2019.10.22, 8 p.m.