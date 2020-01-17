# OpenGL learning notes

## 环境配置相关

+ 创建新项目：
  1. 打开 VS，新建项目

  2. 右键项目名 -> 属性 -> VC++ 目录，在**包含**和**库**两个选项卡中添加依赖项 (glfw-vs)

  3. 右键项目名 -> 属性 -> 链接器 -> 输入，在**附加依赖项**选项卡中添加 opengl32.lib， glfw3.lib 以及 assimp-vc140-mt.lib

  4. 复制一份之前的代码到新项目的目录下（但是这时在 VS 左侧的目录树中并没有这些文件，这也是我一直觉得 VS 比较蠢的地方）

     主要复制的文件有 glad.c, stb_image.h, stb_image.cpp

  5. 右键源文件 -> 添加 -> 现有项，导入这些文件

  6. F5 运行

+ 用 CMake 生成 assimp 的时候要选择 **x64**（默认是 win32）

  这就导致 glfw 得重新生成 x64 版本的，因为我之前用的一直都是 win32

+ 要将 assimp-vc140-mt.dll 放到项目的 x64/Debug 目录下

## Assimp

### Assimp 数据结构

整个模型加载进一个场景（Scene）对象，场景对象包含根节点、网格和材质的信息。

节点（Node）可以理解成一个一个模型，节点组织成一个层级结构，每个节点由许多网格组成。

网格（Mesh）是单独的可绘制实体，没有层级结构，就是一坨存在场景对象中，一个网格包含很多面，每个网格都存有顶点数据、索引（面）和材质信息，其中顶点数据分为顶点位置、法向量和纹理坐标三类。

面（Face）是物体的渲染图元（如三角形），面中存储图元顶点的索引。

每个材质（Material）都包含了一组纹理。

### Mesh 类

自己定义的 Mesh 类便于 OpenGL 识别。

每个 Mesh 对象都是一个**可绘制的实体**，有自己的顶点数据、索引、材质、VAO、VBO、EBO 以及 Draw() 方法。

Mesh 对象在构造时初始化缓冲（VAO, VBO, EBO），渲染（Draw）时采样纹理并绘制。

### Model 类

Model 类包含多个网格，有点类似于 Scene 的角色，因为在将数据从 Assimp 的数据结构转化到我们自己的类的过程中，Node 的层级结构信息被抹去了，Model 类的 processNode() 方法递归地处理所有 Node，将每个 Node 中的 Mesh 不加分别地加载进自己的成员变量中。

processMesh() 方法则是将 Assimp 中 Mesh 的数据结构转化成我们自己定义的 Mesh 类（处理顶点数据、索引以及材质）

Model 对象在构造时先加载 Scene，再递归地调用 processNode() 方法，填充 Mesh 成员变量，渲染时遍历所有 Mesh，调用它们各自的 Draw() 方法。

##### Last-modified date: 2019.11.18, 11 p.m.