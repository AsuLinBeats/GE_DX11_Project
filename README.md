# 基于DX11的软光栅化渲染器

![Language](https://img.shields.io/badge/language-c++-brightgreen)
![Documentation](https://img.shields.io/badge/documentation-yes-brightgreen)
![License](https://img.shields.io/badge/license-MIT-yellow)
## 概述
本项目是一个使用 DirectX 11 实现的软光栅化渲染框架, 项目集成了延迟渲染、基础动画系统、天空盒渲染、简易敌人 AI，以及第一人称相机和基础战斗逻辑等功能。

## 特性
- 基于Windows窗口的第一人称视角控制与玩家移动
- G-buffer实现的延迟渲染框架
- 基础动画系统实现: 换弹动画,开火动画, 敌人移动动画等
- 支持alpha测试的透明模型(树叶等)
- 天空盒渲染
- 基于射线检测的子弹与敌人碰撞系统
- 简单的基于HLSL实现的vertex buffer和pixel buffer处理

## 结构
- main.cpp: 项目主入口，包含初始化逻辑、主循环与渲染管线

- DXCore.h/cpp: DirectX11 初始化与资源管理

- window.h: 窗口创建与输入处理

- mesh.h, character.h, player.h: 模型加载与角色逻辑

- shaders.h: 顶点/像素着色器管理

- Texture.h, TextureManager: 纹理加载与缓存

- camera.h: 摄像机输入与视图矩阵生成

- anime.h: 骨骼动画播放系统

## 待实现的功能
- 水渲染
- 多通道deferred shading
- 

## 技术栈
- C++14
- HLSL
- DX11

外部库使用:
- [GamesEngineeringBase](https://github.com/MSCGamesTom/GamesEngineeringBase)
- [GEM](https://github.com/MSCGamesTom/GEM)
- [Stb Image](https://github.com/nothings/stb)
- 部分基础代码由教师给出

## 调试工具
- StackOverflow
- RenderDoc
- Output & Console
