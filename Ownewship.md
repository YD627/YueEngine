# YueEngine Ownership

## Scope

Scope<T> = std::unique_ptr<T> 表示唯一所有权。

主要用于：
- Application -> Window
- ...

## Ref

Ref<T> = std::shared_ptr<T> 表示共享所有权。

主要用于：
- Shader
- VertexBuffer
- VertexArray
- ...

## Layer

LayerStack 当前使用 Layer*。

需要明确：
- 创建者是：不同的Layer类通过继承基类Layer创建。
- 所有者是：LayerStack。
- 删除者是：LayerStack。
- 为什么没有使用 unique_ptr?
    - 这是一种手动管理所有权的传统 C++ 写法。理论上完全可以使用unique_ptr。