# Homework 6

## Cook-Torrance

文档中给出了 Cook-Torrance 模型中 D, F, G 项的计算公式：

![](./images/ct-1.png)

对应填入 `MyBRDFShader.shader` 文件中的 `GGX_D`, `Schlick_F`, `CookTorrence_G` 函数即可。

材质球矩阵图效果：

![](./images/ct-2.png)

## 素描风格渲染

首先声明 6 张纹理：

![](./images/pencil-1.png)

然后将六张纹理对应的权重存储在两个 fixed3 类型的变量中。

在顶点着色器中首先对顶点进行基本坐标变换，然后逐顶点计算光照，将光照方向和法线方向点乘得到漫反射系数，并将其缩放至 [0, 7] 的区间中，根据其落在哪个子区间计算得到对应纹理的权重。

在片段着色器中对每张纹理进行采样并和对应的权重相乘，将结果相加得到最终的颜色。

![](./images/pencil-2.png)

## 全局雾效

首先在 Camera 的脚本中让其绘制深度图，以便于在 shader 中使用 `_CameraDepthTexture` 变量。然后调用 `Graphics.Blit()` 函数，source 会传给 shader 中的 `_MainTex`，片段着色器的处理结果会传给 destination。

接下来在片段着色器中使用 `SAMPLE_DEPTH_TEXTURE` 对 `_CameraDepthTexture` 采样得到深度值，然后将其和 uv 组合得到坐标，再乘上 `_ClipToCameraMatrix` 就能得到片段在摄像机空间中的位置。算出片段与 Camera 的距离后可以根据 `_FogStart` 和 `_FogEnd` 得到雾的浓度，再由 `_MainTex` 和 uv 的采样结果得到片段原本的颜色，然后根据雾的浓度在该颜色和纯白之间做插值得到加了雾效后的颜色。

![](./images/fog-1.png)

## 操作指南

+ 按 C 开启 / 关闭素描风格渲染

  <div>
      <img src="./images/manual-3.png" width="48%" />
      <img src="./images/manual-4.png" width="48%" />
  </div>

+ 按 F 开启 / 关闭雾效

  <div>
      <img src="./images/manual-1.png" width="48%" />
      <img src="./images/manual-2.png" width="48%" />
  </div>

##### Last-modified date: 2020.4.17, 1 p.m.

