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

![](./images/image1.png)

### Block layer

block number -> block data

+ One superblock per file system

### File layer

inode -> block numbers

inode + offset -> block number

### Inode number layer

inode number -> inode

+ inode number is the *index* of inode table

  ![](./images/image2.png)

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

##### Last-modified date: 2019.9.13, 3 p.m.

