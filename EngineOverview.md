# Yue Engine
> A lightweight real-time rendering engine built with Modern C++ and OpenGL.

---

# 1. 项目简介
Yue Engine 是一个基于Modern C++ 和 OpenGL 开发的小型实时渲染引擎。

本项目最初来源于个人OpenGL游戏项目，随着学习的深入，逐步演化为一个具有独立架构的图像渲染引擎。

项目主要用于：
- 学习现代图形学
- 学习游戏引擎架构
- 学习C++工程设计
- 为未来阅读UE源码做准备
- 作为长期维护的个人作品集

本项目不会追求完全商业化引擎的功能，而是重点关注于：
> “理解为什么这样设计，而并不是简单实现功能。”

---

# 2. 为什么开发这个项目
很多OpenGL教程都会实现：
- 三角形
- 光照
- 阴影
- PBR

但是：这些Demo很少告诉开发者：

为什么需要：Application？

为什么需要Renderer？

为什么需要Resource Manager？

为什么需要Scene？

因此，本项目希望完成：OpenGL Demo → Engine Architecture → Modern Rendering → Game Engine整个成长过程。

---

# 3. 项目目标

短期目标

✓ 建立完整 Engine 架构

✓ 完成 Renderer 模块

✓ 完成 Scene 系统

✓ 完成 Resource Manager

中期目标

✓ Shadow Mapping

✓ HDR

✓ Bloom

✓ PBR

✓ Deferred Rendering

长期目标

✓ ECS

✓ Animation

✓ Editor

✓ GPU Optimization

✓ 学习 Unreal Engine Renderer

---

# 4. 设计原则
整个引擎遵循以下原则：
## 模块化：每个系统负责唯一职责。
例如：Application管理程序生命周期而不是负责渲染。

---

## 可拓展性：新增功能尽量不修改已有模块。
例如：以后新增Deferred Renderer无需修改Application。

---

## 可维护性：所有模块之间保持较低的耦合度。
例如：Camera不直接调用GLFW。Window不直接参与游戏逻辑。

---

## 面向学习
本项目不是为了最快实现功能。而是理解为什么商业引擎这样设计。

---

# 5. 引擎整体架构
```
Engine
│
├── Core
├── Renderer
├── Scene
├── Resource Manager
├── Debug
├── Event
├── Physics
├── UI
├── ImGui
├── ECS
...(未来补充)
```
其中：
- Core 负责生命周期
- Scene 负责场景管理
- Renderer 负责渲染管理
- Resource Manager 负责资源管理

# 6. 引擎模块介绍
## Core
负责：
- Application
- Window
- Time
- Log

---
...(其余模块待完成)

# 7. Version Roadmap
- v0.1: 完成Core部分内容

# 8. 代码风格
命名：
- 类名：PascalCase
- 函数名：camelCase
- 变量名：snake_case
- 常量名：UPPER_CASE
- 成员变量名：m_

---

# 9. 学习记录
每次开发必须回答三个问题：
- 今天学到了什么？
- 为什么这样设计？
- 还有哪些地方可以改进？

所有记录放入：Development.md

---

# 10. 最终目标
希望一年以后，本项目能够成长为：一个具有完整架构的小型实时渲染引擎。它不需要拥有商业引擎的全部功能，但希望能够真正理解：游戏引擎为什么这样设计。现代图形学为什么这样实现。并成为未来学习 Unreal Engine、Unity 以及实时渲染技术的重要基础。