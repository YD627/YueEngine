# Yue Engine Architecture

## 1. Base
Base中定义了引擎中智能指针的**类型别名**和**工厂函数**。

其中：
- Scope: 是unique_ptr的别名，是独占所有权的智能指针。
- Ref: 是shared_ptr的别名，是共享所有权的智能指针。

代码中的语法：
| 语法 | 作用 | 一句话 |
| :--- | :--- | :--- |
| `typename... Args` | 接受任意参数 | "什么都能收" |
| `Args&& args` | 保留值类别 | "左值来左值接，右值来右值接" |
| `std::forward<Args>(args)...` | 还原值类别 | "进来是什么，传出去还是什么" |


## 2. Application
Application 是 Engine 的顶层生命周期管理对象。

负责：
- Window 创建和销毁
- LayerStack 管理
- Loop 循环管理
- Event 处理

## 3. LayerStack
LayerStack（层栈）主要用于管理游戏场景渲染和 UI (如 ImGui) 的更新与绘制顺序。

核心设计思想：Layer vs Overlay
- Layer（普通层）：先入先出。新的 Layer 插入到现有 Layer 的后面，但必须在所有 Overlay 的前面。
- Overlay（覆盖层）：永远保持在栈的最末尾（最后渲染，所以显示在最上层）。
为了实现这一点，代码引入了一个非常巧妙的索引：unsigned int m_LayerInsertIndex = 0;。它像一堵墙，把 m_Layers 这个 vector 分成了两半：[ Layer 0 | Layer 1 | (插入点 m_LayerInsertIndex) | Overlay 0 | Overlay 1 ]

## 4. Renderer

Renderer 负责高层渲染接口。

Renderer
    ↓
RenderCommand
    ↓
RendererAPI
    ↓
OpenGL