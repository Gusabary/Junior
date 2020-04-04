# Homework 5

## Shader 的实现

### 法线 Shader

从顶点着色器中传入法向量，片段着色器中直接将其作为输出

![1586001605636](./images/1586001605636.png)

### 纹理 Shader

在顶点着色器中增加纹理坐标信息，并在片段着色器中输出采样结果

![1586001753217](./images/1586001753217.png)

### Blinn-Phong 光照 Shader

在片段着色器中输出环境光、漫反射光和镜面光的叠加

![1586001715192](./images/1586001715192.png)

## Shader GUI 的实现

在 Shader 最后声明所要使用的自定义 GUI，然后从 ShaderGUI 类继承出自定义的 GUI 类

选择 Use Specular 为 true 可以设置镜面高光的参数

![1586001996410](./images/1586001996410.png)

## Debug 工具

选用的是 Unity Frame Debugger，可以通过 Window > Analysis > Frame Debugger 打开窗口，并点击 Enable 开启，对于每一个 Draw Mesh，都是渲染过程中的步骤，还可以通过 ShaderProperties 查看到 Shader 中属性的值。

![1586002151169](./images/1586002151169.png)

## 描边 Shader 的实现

在原先 Shader 的基础上多加一个 Pass，在这个 Pass 的顶点着色器中将对象的顶点放大一个系数，然后在片段着色器中输出描边的颜色，最后再加上 Cull Front 绘制靠后的片段以达到深度反转的效果，这样新加的 Pass 绘制出来的片段只有边上一圈会显示。

![1586002597429](./images/1586002597429.png)

## 操作指南

+ WSAD 控制角色移动
+ 移动鼠标控制玩家视野朝向
+ 空格跳跃
+ 点击鼠标左键创建草方块
+ 点击鼠标右键摧毁草方块

*已知有一个 bug，就是创建或者摧毁的时候点击某些面会不起作用，可能是 OnMouseOver 函数的原因*

##### Last-modified date: 2020.4.4, 8 p.m.