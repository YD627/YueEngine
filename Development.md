# 项目目录

```text
Engine
│
├── Core
│   ├── Application.h
│   ├── Application.cpp
│   ├── Window.h
│   ├── Window.cpp
│   ├── Time.h
│   ├── Time.cpp
│   ├── Log.h
│   └── Log.cpp
│
└── main.cpp
```

---

# 开发日志

## 2026-06-09

### 今日目标

开始将原有 OpenGL 游戏项目重构为可长期维护的个人图形学/游戏引擎。

本次开发重点为 Core 模块的基础搭建。

---

### 完成内容

#### 1. Application 模块

新增：

```cpp
Application.h
Application.cpp
```

职责：

* 引擎生命周期管理
* 主循环管理
* 初始化核心模块
* 程序退出管理

目前主程序入口已简化为：

```cpp
int main()
{
    Application app;
    app.Run();

    return 0;
}
```

实现了引擎入口与业务逻辑解耦。

---

#### 2. Window 模块

新增：

```cpp
Window.h
Window.cpp
```

职责：

* GLFW 窗口创建
* OpenGL Context 初始化
* 事件轮询
* Buffer Swap

封装前：

```cpp
glfwCreateWindow(...)
glfwSwapBuffers(...)
glfwPollEvents(...)
```

直接出现在 main.cpp。

封装后：

```cpp
m_Window->Update();
```

Application 不再依赖 GLFW 具体实现。

架构关系：

```text
Application
    ↓
 Window
    ↓
  GLFW
```

---

#### 3. Time 模块

新增：

```cpp
Time.h
Time.cpp
```

职责：

* DeltaTime 计算
* 总运行时间统计
* 未来 FPS 统计支持

封装前：

```cpp
float deltaTime;
float lastFrame;
```

全局变量管理。

封装后：

```cpp
Time::DeltaTime();
Time::TotalTime();
```

实现统一时间系统。

示例：

```cpp
position += velocity * Time::DeltaTime();
```

---

### 4. Log 模块

新增：

```cpp
Log.h
Log.cpp
```

职责：

* 统一日志输出
* 调试信息记录
* 错误信息记录

支持：

```cpp
Log::Info(...)
Log::Warn(...)
Log::Error(...)
```

输出格式：

```text
[INFO][14:23:15] Window Created
[WARN][14:23:18] Texture Missing
[ERROR][14:23:20] Shader Compile Failed
```

重构内容：

将重复的时间获取逻辑封装至：

```cpp
GetCurrentTime()
```

将重复的日志输出逻辑封装至：

```cpp
Print(...)
```

减少代码重复，提高可维护性。

---

### 架构变化

原始结构：

```text
main.cpp
 ├── GLFW
 ├── DeltaTime
 ├── Log
 └── Game Logic
```

当前结构：

```text
Application
 ├── Window
 ├── Time
 └── Log
```

开始形成基础引擎架构。

---

### 遇到的问题

#### localtime 编译警告

Visual Studio 对：

```cpp
localtime()
```

产生 C4996 警告。

解决方案：

```cpp
localtime_s(...)
```

替代传统实现。

提高线程安全性与平台兼容性。

---

### 下一阶段计划

Core 模块：

* [ ] Input
* [ ] Event System
* [ ] Layer System

Renderer 模块：

* [ ] Shader
* [ ] Texture
* [ ] Mesh
* [ ] Renderer

目标：

将现有 OpenGL 游戏项目逐步迁移至 Engine 架构下运行。

---

### 开发感悟

今天首次完成了从“课程项目代码”向“引擎代码”的结构拆分。

虽然功能没有增加，但项目的可维护性显著提升。

后续开发重点将从“继续堆功能”转向“建立稳定架构”，为未来加入 PBR、ECS、动画系统和编辑器功能做准备。


## 2026-07-27
### 今日目标
完成引擎概述文档和架构文档。

---

## 2026-07-28(Window 系统架构重构)

### 今日目标

完成 Window 系统架构重构。

将原本直接依赖 GLFW 的窗口实现，
升级为 Engine 层 Window 抽象接口 + Platform 层具体实现的结构。

本次开发重点：

- 理解平台抽象设计
- 解耦 Application 与 GLFW
- 建立 Core 与 Platform 分层结构

---

### 完成内容

#### 1. Window 抽象层设计

将原本 Window 类重新设计为 Engine 核心接口。

新增：

```cpp
Core/Window.h
Core/Window.cpp
```
Window 不再负责具体窗口创建逻辑，而只定义窗口系统需要提供的基础能力。

目前提供：
```cpp
virtual void Update() = 0;

virtual bool ShouldClose() const = 0;

virtual unsigned int GetWidth() const = 0;

virtual unsigned int GetHeight() const = 0;
```
同时增加：
```cpp
static Window* Create(const WindowProps& props = WindowProps());
```
用于隐藏具体平台实现。

#### 2. 创建 Platform 层 GLFW 实现
新增：
```
Platform
└── GLFW
    ├── GLFWWindow.h
    └── GLFWWindow.cpp
```
GLFWWindow 继承 Window 接口：
```cpp
class GLFWWindow : public Window
```
负责具体 GLFW 平台相关操作：
- GLFW 初始化
- 创建窗口
- OpenGL Context 创建
- Event Poll
- Buffer Swap

#### 3. Application 职责重新划分
重新明确 Application 的职责。

Application 不再负责：
```cpp
glfwInit();
glfwCreateWindow();
glfwPollEvents();
glfwTerminate();
```
这些平台相关代码全部移动至：Platform/GLFW/GLFWWindow

Application 只负责：
- Engine 生命周期管理
- 主循环控制
- 系统初始化
- 系统销毁

最终目标：
```cpp
int main()
{
    Application app;
    app.Run();
}
```
保持程序入口简单。

### 设计思考
1. 为什么需要 Window 抽象？
```
如果 Application 直接依赖 GLFW：
Application
↓
GLFW

那么未来更换窗口系统：
例如：
SDL
Win32
MacOS Native Window

都需要修改 Application。

通过抽象：
Application
↓
Window
↓
Platform Implementation
可以将平台差异隔离。
```

2. Core 与 Platform 分离
```
Core 模块只负责：Engine 通用逻辑。

Platform 模块负责：操作系统和第三方库相关实现。

未来可以扩展：
Platform
├── GLFW
├── SDL
└── Win32
而不影响 Engine 核心代码。
```

---

## 2026-07-29(Input 系统)
### 今日目标
完成 Input 系统架构重构。

设计一个Input接口，再设计GLFWInput实现。让Input中只负责查询输入状态，而隐藏窗口和平台细节。

---

### 完成内容
#### 1. Input 接口设计
Input类中均使用static函数，因为输入系统通常是全局统一的，不需要实例化。

函数：
```cpp
static bool IsKeyPressed(KeyCode key);
static bool IsMouseButtonPressed(MouseCode button);
static glm::vec2 GetMousePosition();
static float GetMouseX();
static float GetMouseY();
```
实现了查询用户是否按下了某个按键或点击了某个鼠标按钮。此外，还提供了获取鼠标位置的函数。

#### 2. GLFWInput 实现
GLFWInput.cpp头文件：
```cpp
#include "GLFWInput.h"
#include "GLFW/glfw3.h"
```

实现了 Input 接口的函数。其中得到窗口信息的代码暂时如下：
```cpp
auto window = static_cast<GLFWwindow*>(glfwGetCurrentContext());
```
其余方法都是调用 GLFW 函数。例如：
```cpp
auto state = glfwGetKey(window, key);
return state == GLFW_PRESS;
```
用于查询用户是否按下了某个按键 key。

---

### 设计思考
1. 为什么需要 Input 接口？
```
如果游戏代码直接依赖 GLFW 的输入宏，
那么游戏代码就会直接依赖于 GLFW 的实现，
而不是通过 Input 接口来查询输入状态。
```

2. Input 接口与 GLFWInput 实现的分离
```
Input 接口定义了输入系统需要提供的基础能力，
而 GLFWInput 实现则负责具体 GLFW 平台相关操作。
分离后，可以方便地更换平台实现，
而不会修改游戏代码。
```

---

## 2026-07-30(Event 系统)
### 今日目标
完成Event系统的初步设计。实现了窗口关闭事件。

设计了一个Event接口，让其它的事件基于这个接口来实现。

---

### 完成内容
#### 1. Event 接口设计
1. 事件类型菜单
    ```cpp
    enum class EventTypes
    {
	    None = 0,
	    WindowClose,
	    WindowResize,
	    KeyPressed,
	    KeyReleased,
	    MouseButtonPressed,
    };
    ```
    使用class来定义事件类型，因为事件类型是不可变的。同时，使用enum class来定义事件类型，因为事件类型是枚举值。这样可以避免事件类型之间的冲突。

#### 2. ApplicationEvent.h 设计
1. 窗口关闭事件类
    ```cpp
    class WindowCloseEvent : public Event
    {
    public:
        EventTypes GetEventType() const override {
            return EventTypes::WindowClose;
        }
    };
    ```
    窗口关闭事件类继承自Event接口，用于表示窗口关闭事件。其中，GetEventType()函数返回事件类型。

#### 3. Window.h 添加和事件相关的函数
1. using EventCallbackFn = std::function<void(Event&)>;
    > 它不是函数，不是变量，是一个类型标签，用来统一描述"事件回调长什么样"，让代码更短、改起来只改一处。

2. virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    > 这行代码的意思是："我（基类）规定你必须有一个设置事件回调的能力，但具体怎么做我不管，你（子类）自己实现。" 它强制所有平台窗口类都提供这个函数，同时让上层代码可以统一通过基类指针调用。

#### 4. GLFWWindow.h 实现SetEventCallback
```cpp
void SetEventCallback(const EventCallbackFn& callback) override 
{ 
    m_Data.EventCallback = callback; 
}
```
> 这行代码的意思是："设置事件回调函数。" 它将用户传递的回调函数存储起来，后续在事件发生时调用。

#### 5. Application.h 添加事件函数
1. void OnEvent(Event& event);
    > 这行代码的意思是："处理事件。" 它在主循环中调用，用于处理事件。
    > 事件系统的好处是：OnEvent(Event&) 把 "事件从哪来" 和 "事件怎么处理" 彻底解耦。窗口只管生产事件，上层通过一个统一入口按优先级分发、拦截、消费。加事件不改框架，加处理不改窗口，事件流转顺序清晰可控。

#### 6. Application.cpp 实现OnEvent
```cpp
void Application::OnEvent(Event& event)
{
    if (event.GetEventType() == EventTypes::WindowClose)
    {
        m_Running = false;
    }
}
```
> 这行代码的意思是："处理事件。" 它根据事件类型，调用对应的处理函数。

#### 7. GLFWWindow.cpp 设置事件回调
```cpp
glfwSetWindowUserPointer(m_Window, &m_Data);

glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
	WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
	WindowCloseEvent event;
	data.EventCallback(event);
});
```
这段代码首先通过glfwSetWindowUserPointer()函数将窗口数据指针存储到窗口句柄中。然后，通过glfwSetWindowCloseCallback()函数设置窗口关闭回调函数。在回调函数中，通过glfwGetWindowUserPointer()函数获取窗口数据指针，再调用用户传递的回调函数。最后，调用OnEvent()函数处理事件。EventCallback的地址和OnEvent()函数的地址是同一个，因为OnEvent()函数是Application类的成员函数，而EventCallback的地址是Application类的指针。所以，需要在回调函数中使用this->OnEvent(event)来调用OnEvent()函数。

### 难点
1. 搞不清事件回调函数的顺序问题
    我一开始误以为事件函数要在主函数中一直监听，但是实际上，事件函数是在窗口关闭回调函数中调用的。然后我一开始也不能理解为什么窗口关闭时OnEvent函数就会被调用。仔细阅读代码后发现一开始的SetEventCallback()函数在Application.cpp中掉用时就将窗口的事件回调函数的地址针指向了OnEvent()函数的地址针。所以，窗口关闭时，窗口关闭回调函数就会调用OnEvent()函数，从而触发窗口关闭事件。

2.  为什么用 enum class 而不是普通 enum 或 int 来定义事件类型？
    事件类型是枚举值，所以用enum class来定义事件类型。同时，enum class可以避免事件类型之间的冲突。

3. using EventCallbackFn = std::function<void(Event&)> 到底在定义什么？
    它不是函数，不是变量，是一个类型标签，用来统一描述"事件回调长什么样"，让代码更短、改起来只改一处。

4. virtual void SetEventCallback(const EventCallbackFn& callback) = 0; 中 = 0 的含义是？
    它是虚函数的实现，表示子类必须实现这个函数。

5. 为什么 Application 只用一个 OnEvent(Event&) 而不是每种事件一个回调？
    事件系统的好处是：OnEvent(Event&) 把 "事件从哪来" 和 "事件怎么处理" 彻底解耦。窗口只管生产事件，上层通过一个统一入口按优先级分发、拦截、消费。加事件不改框架，加处理不改窗口，事件流转顺序清晰可控。

6. GLFW 回调中 glfwSetWindowUserPointer 的作用是？
    GLFW 的回调函数签名是固定的 C 风格函数指针：
    ```cpp
    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
    // 这里拿不到 GLFWWindow 的 this，也拿不到 m_Data
    });
    ```
    问题：lambda 内部需要访问 m_Data.EventCallback，但 GLFW 只传了一个 GLFWwindow*。

    解决方案：提前把 m_Data 的地址"藏"进窗口句柄：
    ```cpp
    glfwSetWindowUserPointer(m_Window, &m_Data);
    ```
    回调触发时再取出来：
    ```cpp
    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);  // 取出来
        WindowCloseEvent event;
        data.EventCallback(event);  // 调用 Application::OnEvent()函数
    });
    ```
    本质是用 GLFW 提供的"用户数据槽"桥接了 C 回调与 C++ 对象成员之间的鸿沟。

---

## 2026-07-31(EventDispatcher)

### 今日目标

完成 EventDispatcher 类，实现事件的类型分发机制。
新增 WindowResize 事件，验证 Dispatcher 的可扩展性。

---

### 完成内容

#### 1. EventDispatcher 类设计

在 `Event.h` 中新增 EventDispatcher：

```cpp
class EventDispatcher {
public:
    EventDispatcher(Event& e) : m_Event(e) {}

    template<typename T, typename F>
    bool Dispatch(const F& func) {
        if (m_Event.GetEventType() == T::GetStaticType()) {
            m_Event.Handled |= func(static_cast<T&>(m_Event));
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};
```

职责：
- 接收一个基类 `Event&` 引用
- 通过模板参数 `T` 判断事件实际类型
- 类型匹配时，将基类引用 `static_cast` 为派生类引用，交给回调处理
- 通过 `Handled` 标记实现事件消费/拦截机制

#### 2. 为 Event 基类添加静态类型查询

```cpp
class Event {
public:
    virtual EventTypes GetEventType() const = 0;  // 运行时：问对象
    static EventTypes GetStaticType();             // 编译期：问类别
    bool Handled = false;
};
```

每个派生事件类都必须提供自己的 `GetStaticType()`：

```cpp
static EventTypes GetStaticType() { return EventTypes::WindowResize; }
```

> `GetStaticType()` 必须是 static 的，因为 Dispatch 做类型判断时手里只有基类引用，还没有派生类对象，无法调用非静态成员。

#### 3. 新增 WindowResizeEvent

在 `ApplicationEvent.h` 中新增：

```cpp
class WindowResizeEvent : public Event {
public:
    WindowResizeEvent(unsigned int width, unsigned int height)
        : m_Width(width), m_Height(height) {}

    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }

    EventTypes GetEventType() const override { return EventTypes::WindowResize; }
    static EventTypes GetStaticType() { return EventTypes::WindowResize; }

private:
    unsigned int m_Width, m_Height;
};
```

与 WindowCloseEvent 不同，WindowResizeEvent 携带数据（宽高），通过构造函数注入。

#### 4. GLFWWindow 注册窗口大小回调

在 `GLFWWindow.cpp` 的 `Init()` 中新增：

```cpp
glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
    WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
    data.Width = width;
    data.Height = height;
    WindowResizeEvent event(width, height);
    data.EventCallback(event);
});
```

窗口大小改变时，同步更新 `WindowData` 中的宽高，并构造 `WindowResizeEvent` 向上层分发。

#### 5. Application 使用 EventDispatcher 重构事件处理

重构前（7-30 的写法）：

```cpp
void Application::OnEvent(Event& event) {
    if (event.GetEventType() == EventTypes::WindowClose) {
        m_Running = false;
    }
}
```

重构后：

```cpp
void Application::OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& event) {
        return OnWindowClose(event);
    });
    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event) {
        return OnWindowResize(event);
    });
}
```

新增处理函数：

```cpp
bool Application::OnWindowClose(WindowCloseEvent& e) {
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e) {
    std::cout << "Resize: " << e.GetWidth() << " " << e.GetHeight() << std::endl;
    return true;
}
```

---

### 设计思考

#### 1. 为什么用 EventDispatcher 替代 if-else？

```text
重构前：每加一种事件 → OnEvent 里加一个 if 分支
重构后：每加一种事件 → 加一行 Dispatch<T>(...) 即可
```

- 类型判断和类型转换封装在 Dispatcher 内部
- 新增事件不需要修改分发逻辑，只需注册新的 Dispatch 行
- 回调函数签名统一为 `bool(T&)`，返回值控制事件是否被消费

#### 2. 为什么 Dispatch 用模板而不是虚函数？

```cpp
template<typename T, typename F>
bool Dispatch(const F& func)
```

- 事件类型在编译期确定（`Dispatch<WindowResizeEvent>`），无需运行时多态
- `static_cast<T&>` 是向下转型，模板参数 `T` 让编译器知道目标类型
- 回调 `F` 使用模板推导，兼容 lambda、函数指针、std::function 等任意可调用对象

#### 3. 为什么回调要捕获 `[this]`？

```cpp
[this](WindowResizeEvent& event) {
    return OnWindowResize(event);
}
```

`OnWindowResize` 是成员函数，隐含需要对象实例。Lambda 通过 `[this]` 捕获当前 Application 指针，将"对象 + 事件"两个参数绑定为只需传入"事件"一个参数，从而匹配 Dispatch 的回调签名。

#### 4. `Handled |= func(...)` 的意义

```cpp
m_Event.Handled |= func(static_cast<T&>(m_Event));
```

- 使用 `|=` 而非 `=`：一旦事件被某个处理器消费，后续处理器不会覆盖该状态
- 为未来事件冒泡/拦截机制预留：上层可检查 `e.Handled` 决定是否继续传递

---

### 架构变化

事件流转路径：

```text
GLFW 回调触发
    ↓
GLFWWindow 构造具体 Event（如 WindowResizeEvent）
    ↓
data.EventCallback(event)  →  Application::OnEvent(Event&)
    ↓
EventDispatcher 按类型匹配
    ↓
对应的 OnWindowResize / OnWindowClose 处理
```

---

## 2026-08-03(Layer)
---

### 今日目标

完成 Layer 系统的初步搭建。
设计 Layer 基类作为所有逻辑层的抽象接口，设计 LayerStack 容器统一管理层的增删、排序与遍历。
将 Layer 系统接入 Application 主循环与事件系统，验证基本流转。

### 完成内容

#### 1. Layer 基类设计

新增：

- `Layer.h`
- `Layer.cpp`

Layer 是所有逻辑层的抽象基类，定义了层在引擎中的四个生命周期钩子：

```cpp
class Layer
{
public:
    Layer(const std::string& name = "Layer");
    virtual ~Layer() {}
    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate() {}
    virtual void OnEvent(Event& event) {};
protected:
    std::string m_DebugName;
};
```

各函数职责：

| 函数 | 调用时机 | 职责 |
|------|---------|------|
| `OnAttach()` | 层被加入 LayerStack 时 | 初始化资源 |
| `OnDetach()` | 层被移除或引擎销毁时 | 释放资源 |
| `OnUpdate()` | 每帧主循环 | 执行逻辑更新 |
| `OnEvent()` | 事件产生时 | 处理/拦截事件 |

`m_DebugName` 用于调试时标识当前层，方便日志输出和问题定位。

#### 2. LayerStack 容器设计

新增：

- `LayerStack.h`
- `LayerStack.cpp`

LayerStack 是 Layer 的统一管理器，核心结构：

```cpp
std::vector<Layer*> m_Layers;
unsigned int m_LayerInsertIndex = 0;
```

通过 `m_LayerInsertIndex` 将容器划分为两个区域：

```
m_Layers: [ 普通层A, 普通层B, | 覆盖层X, 覆盖层Y ]
                              ↑
                     m_LayerInsertIndex
```

提供两组操作：

```cpp
void PushLayer(Layer* layer);      // 插入到 m_LayerInsertIndex 位置（普通层区域）
void PushOverlay(Layer* overlay);  // 追加到末尾（覆盖层区域）
void PopLayer(Layer* layer);       // 从普通层区域移除
void PopOverlay(Layer* overlay);   // 从覆盖层区域移除
```

普通层（游戏逻辑、场景等）始终在覆盖层（UI、调试面板等）之前，保证更新顺序和事件优先级正确。

同时提供完整的迭代器接口（含 const 版本），方便外部遍历：

```cpp
std::vector<Layer*>::iterator begin();
std::vector<Layer*>::iterator end();
std::vector<Layer*>::reverse_iterator rbegin();
std::vector<Layer*>::reverse_iterator rend();
```

析构函数中统一调用 `OnDetach()` 并释放所有层：

```cpp
LayerStack::~LayerStack()
{
    for (Layer* layer : m_Layers)
    {
        layer->OnDetach();
        delete layer;
    }
}
```

#### 3. Application 接入 Layer 系统

修改：

- `Application.h`
- `Application.cpp`

**新增成员：**

```cpp
LayerStack m_LayerStack;
```

**构造函数中注册测试层：**

```cpp
m_LayerStack.PushLayer(new TestLayer());
```

**事件分发至各层：**

在 `OnEvent` 中，Application 先处理引擎级事件（WindowClose、WindowResize），再将事件从后往前传递给各层：

```cpp
for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
{
    (*--it)->OnEvent(e);
    if (e.Handled) break;
}
```

从后往前遍历意味着覆盖层（如 UI）优先处理事件。若某层将 `e.Handled` 置为 `true`，事件不再继续向下传递，实现事件拦截机制。

#### 4. TestLayer 验证

在 `Application.h` 中新增测试层，验证 Layer 系统基本流转：

```cpp
class TestLayer : public Layer
{
public:
    void OnAttach() override
    {
        std::cout << "TestLayer attached" << std::endl;
    }
    void OnUpdate() override
    {
        std::cout << "TestLayer updated" << std::endl;
    }
};
```

### 设计思考

#### 1. 为什么需要 Layer？

如果所有游戏逻辑、UI、调试信息都写在 Application 的主循环中，代码会迅速膨胀且难以维护。Layer 将不同职责的逻辑拆分为独立单元，引擎只需按统一接口驱动它们，无需关心具体实现。

#### 2. 为什么需要 LayerStack 而不是直接用 vector？

直接用 `vector<Layer*>` 需要手动管理：
- 普通层和覆盖层的插入位置
- 分界索引的维护
- 移除时的查找范围和索引修正
- 析构时的统一清理

LayerStack 将这些逻辑封装为内聚的类，避免外部代码直接操作底层容器，降低出错概率。

#### 3. 为什么事件从后往前传递？

```cpp
for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
```

覆盖层（如 UI、调试面板）在视觉和交互上位于最顶层，应当优先接收事件。例如：用户点击了一个 UI 按钮，该事件不应再传递给下层的场景逻辑。从后往前遍历 + `Handled` 拦截保证了这一优先级。

#### 4. PushLayer 中 OnAttach 被注释的原因

```cpp
void LayerStack::PushLayer(Layer* layer)
{
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
    // layer->OnAttach();
}
```

当前阶段 OnAttach 的调用时机尚未最终确定（是否应在 Application 统一调用、是否需要延迟初始化等），暂时注释，后续明确后再启用。

### 架构变化

事件流转路径更新：

```
GLFW 回调触发
    ↓
GLFWWindow 构造具体 Event
    ↓
Application::OnEvent(Event&)
    ↓
EventDispatcher 处理引擎级事件（WindowClose / WindowResize）
    ↓
LayerStack 从后往前逐层分发
    ↓
各 Layer::OnEvent(Event&)（支持 Handled 拦截）
```

当前引擎结构：

```
Application
 ├── Window（平台抽象）
 ├── LayerStack
 │    ├── Layer（普通层）
 │    └── Overlay（覆盖层）
 ├── EventDispatcher
 ├── Time
 └── Log
```

### 遇到的问题

**反向迭代器遍历写法**

从后往前遍历 `vector` 时，不能直接用 `rbegin/rend` 配合 `break`（因为需要精确控制 `--` 时机），最终采用手动递减正向迭代器的方式：

```cpp
for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
{
    (*--it)->OnEvent(e);
    if (e.Handled) break;
}
```

先 `--it` 再解引用，避免越界访问 `end()`。

### 下一阶段计划

- [ ] 启用 `OnAttach()` 调用时机
- [ ] 在 `Run()` 主循环中遍历调用各层 `OnUpdate()`
- [ ] 将 Input 系统的事件（KeyPressed、MouseButtonPressed 等）接入 Layer 事件流
- [ ] 设计 Sandbox 层，将游戏逻辑从 Application 中分离
- [ ] Renderer 模块初步搭建

---

以下是今日的开发日志：

---


## 2026-08-04（Sandbox 项目分离与工厂模式）

### 今日目标

将引擎（Yue）与用户代码（Sandbox）彻底分离为两个独立项目。
引入 `CreateApplication()` 工厂函数与 `EntryPoint.h` 统一入口，实现引擎与客户端的完全解耦。
将原本内嵌在 `Application.h` 中的 `TestLayer` 迁移至独立的 Sandbox 项目中。

---

### 完成内容

#### 1. 项目结构拆分

将原本单一项目拆分为两个独立项目：

```
Solution
├── Yue（引擎库，输出静态库 .lib）
│   ├── Core/
│   │   ├── Application.h / .cpp
│   │   ├── EntryPoint.h
│   │   ├── Window.h / .cpp
│   │   ├── Layer.h / .cpp
│   │   ├── LayerStack.h / .cpp
│   │   ├── Event/
│   │   └── ...
│   └── Platform/
│       └── GLFW/
│
└── Sandbox（客户端应用，输出可执行文件 .exe）
    ├── SandboxApp.cpp
    ├── SandboxLayer.h
    └── SandboxLayer.cpp
```

#### 2. CreateApplication() 工厂函数

**Application.h 新增声明：**

```cpp
namespace Yue {
    // 在客户端定义这个函数，返回一个Application的实例
    Application* CreateApplication();
}
```

**SandboxApp.cpp 中实现：**

```cpp
Yue::Application* Yue::CreateApplication()
{
    return new SandboxApp();
}
```

引擎只声明该函数，具体实现由客户端项目提供。链接器在链接阶段将两者绑定。

#### 3. EntryPoint.h 统一入口

新增 `EntryPoint.h`，作为引擎提供的程序入口：

```cpp
#pragma once
#include "Application.h"

extern Yue::Application* Yue::CreateApplication();

int main()
{
    auto app = Yue::CreateApplication();
    app->Run();
    delete app;
}
```

客户端只需 `#include "Yue/Core/EntryPoint.h"` 即可获得 `main()`，无需自行编写入口逻辑。

#### 4. SandboxApp 客户端应用

新增 `SandboxApp.cpp`：

```cpp
#include "Yue/Core/EntryPoint.h"
#include "SandboxLayer.h"

class SandboxApp : public Yue::Application
{
public:
    SandboxApp()
    {
        PushLayer(new SandboxLayer());
    }
};
```

`SandboxApp` 继承 `Yue::Application`，在构造函数中注册自定义层。
引擎的 `main()` 通过 `CreateApplication()` 获取该实例并驱动其运行。

#### 5. SandboxLayer 自定义层

新增 `SandboxLayer.h` / `SandboxLayer.cpp`：

```cpp
class SandboxLayer : public Yue::Layer
{
public:
    SandboxLayer();
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(Yue::Event& event) override;
};
```

实现中通过构造函数设置调试名称：

```cpp
SandboxLayer::SandboxLayer() : Layer("SandboxLayer") {}
```

各生命周期函数目前输出调试信息，验证 Layer 系统的完整流转。

#### 6. Application 修改

- **移除**了原本内嵌在 `Application.h` 中的 `TestLayer` 类
- **移除**了构造函数中的 `m_LayerStack.PushLayer(new TestLayer())`
- **启用**了 `PushLayer()` / `PushOverlay()` 中的 `OnAttach()` 调用（之前被注释）：

```cpp
void Application::PushLayer(Layer* layer) {
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();  // ← 正式启用
}

void Application::PushOverlay(Layer* overlay) {
    m_LayerStack.PushOverlay(overlay);
    overlay->OnAttach();  // ← 正式启用
}
```

---

### 设计思考

#### 1. 为什么需要 CreateApplication() 工厂函数？

| 方案 | 问题 |
|------|------|
| 引擎 `main()` 直接 `new Application()` | 引擎无法知道用户要创建什么子类 |
| 引擎 `#include` 用户的头文件 | 引擎依赖用户代码，耦合方向反转 |
| **工厂函数（当前方案）** | 引擎只声明接口，用户提供实现，链接时绑定 |

引擎完全不感知 Sandbox 的存在，Sandbox 单向依赖引擎。

#### 2. 为什么 EntryPoint.h 用 `extern` 而不是 `virtual`？

```cpp
extern Yue::Application* Yue::CreateApplication();
```

- 这是一个**自由函数**，不是成员函数，无法用虚函数机制
- `extern` 告诉编译器："这个函数在别的编译单元中定义，链接时再找"
- 链接器会将 Sandbox 项目中的实现与 Yue 项目中的调用绑定

#### 3. 为什么 Sandbox 不包含 `main()`？

`main()` 由引擎的 `EntryPoint.h` 提供。客户端只需包含该头文件，引擎控制程序生命周期：

```
EntryPoint.h (引擎提供 main)
    → CreateApplication() (用户实现)
    → app->Run() (引擎驱动)
    → delete app (引擎清理)
```

用户永远不需要写 `main()`，只需实现 `CreateApplication()` 并注册自己的 Layer。

#### 4. OnAttach() 为什么在 PushLayer 中调用而不是在 Run() 中？

在 `PushLayer` 中调用意味着层被注册时立即初始化，无论主循环是否已经开始。这允许用户在 `Run()` 之前完成资源准备。

---

### 架构变化

**之前（08-03）：**
```
Application（单项目）
 ├── Window
 ├── LayerStack
 │    └── TestLayer（内嵌在 Application.h 中）
 ├── EventDispatcher
 └── main() 写在 Application.cpp 或独立文件中
```

**现在（08-04）：**
```
Yue 引擎项目（静态库）
 ├── Application
 │    ├── Window
 │    ├── LayerStack（空，等待用户注册）
 │    ├── EventDispatcher
 │    └── CreateApplication() 声明
 └── EntryPoint.h（提供 main()）

Sandbox 客户端项目（可执行文件）
 ├── SandboxApp : Application
 │    └── PushLayer(new SandboxLayer())
 ├── SandboxLayer : Layer
 └── CreateApplication() 实现
```

**依赖方向：**
```
Sandbox → Yue（单向依赖，引擎不感知客户端）
```

---

### 遇到的问题

#### Include 路径传递性问题

Sandbox 项目包含 `Layer.h` 时，`Layer.h` 内部的 `#include "Event/Event.h"` 无法被找到。

**原因：** 编译器处理 Sandbox 项目时，只使用 Sandbox 的 Include 路径配置，不会继承 Yue 项目的路径。

**解决：** 在 Sandbox 项目的附加包含目录中同时添加：
- `$(SolutionDir)` → 解析 `#include "Yue/Core/Layer.h"`
- `$(SolutionDir)Yue\` → 解析 `Layer.h` 内部的 `#include "Event/Event.h"`

---

### 下一阶段计划

- [ ] 在 `Run()` 主循环中遍历调用各层 `OnUpdate()`
- [ ] 将 Input 事件（KeyPressed、MouseButtonPressed）接入 Layer 事件流
- [ ] 在 SandboxLayer 中接入 Input 查询，验证交互
- [ ] Renderer 模块初步搭建（Shader、Buffer）
- [ ] 考虑 Overlay 的实际使用场景（如 ImGui 调试层）
    
---

## 2026-08-05(Log系统和Base.h)

### 今日目标

引入智能指针别名与工厂函数，完成引擎内存管理现代化改造；搭建独立日志系统；创建 SandboxLayer 验证 Layer 系统在客户端项目中的完整流转。

---

### ✅ 完成内容

#### 1. Base.h — 智能指针别名与完美转发工厂

新增 `Base.h`，定义了引擎统一的智能指针类型别名和创建函数：

```cpp
template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}
```

**关键知识点实践：**
- `std::forward<Args>(args)...` 完美转发：保留调用者传入参数的左值/右值属性，避免不必要的拷贝
- 参数包展开 `...`：支持任意数量、任意类型的构造参数透传
- `Scope` / `Ref` 别名：统一引擎内存管理语义，`Scope` 表示独占所有权，`Ref` 表示共享所有权

#### 2. Window 系统接入 Scope

**Window.h** 修改：
- `Create()` 返回值从裸指针改为 `Scope<Window>`
- 包含 `Base.h`

**Window.cpp** 修改：
```cpp
Scope<Window> Window::Create(const WindowProps& props) {
    return CreateScope<GLFWWindow>(props);
}
```
窗口创建现在通过 `CreateScope` 工厂函数完成，返回 `unique_ptr`，消除裸指针泄漏风险。

### 3. Application 接入 Scope

**Application.h** 修改：
- 成员 `m_Window` 从裸指针改为 `Scope<Window>`
- 包含 `Base.h`

**Application.cpp** 修改：
- 构造函数中使用 `Window::Create()` 直接赋值给 `Scope<Window>`
- 所有 `m_Window->` 调用保持不变（`unique_ptr` 重载了 `->` 运算符）

#### 4. Log 日志系统

**Log.h** 新增：
```cpp
class Log {
public:
    static void Init();
    static void CoreInfo(const std::string& message);
    static void Info(const std::string& message);
};

#define YUE_CORE_INFO(message) ::Yue::Log::CoreInfo(message)
#define YUE_INFO(message) ::Yue::Log::Info(message)
```

**Log.cpp** 实现：
- `CoreInfo` 输出 `[Yue Engine]` 前缀，用于引擎内部日志
- `Info` 输出 `[APP]` 前缀，用于客户端应用日志
- 宏封装简化调用语法，同时保留命名空间限定避免冲突

**Application.cpp** 中首次使用：
```cpp
Log::Init();
YUE_CORE_INFO("Application Created");
```

#### 5. SandboxLayer 独立验证层

**SandboxLayer.cpp** 实现完整的 Layer 生命周期钩子：
```cpp
SandboxLayer::SandboxLayer() : Layer("SandboxLayer") {}
void SandboxLayer::OnAttach()  { YUE_INFO("Sandbox Attach"); }
void SandboxLayer::OnDetach()  { YUE_INFO("Sandbox Detach"); }
void SandboxLayer::OnUpdate()  { YUE_INFO("Sandbox Update"); }
void SandboxLayer::OnEvent(Yue::Event& event) {}
```

- 使用 `YUE_INFO` 宏验证日志系统在客户端项目中正常工作
- 各生命周期函数均已实现，验证 LayerStack 的 PushLayer → OnAttach 流程

---

### 🧠 今日核心知识点总结

| 知识点 | 要点 |
|--------|------|
| **完美转发** | `std::forward<T>(arg)` 根据模板参数 `T` 有条件地恢复值类别：`T=U&` → 左值，`T=U` → 右值 |
| **std::move 本质** | 仅做 `static_cast<T&&>` 类型转换，不移动任何数据；真正的移动由移动构造/赋值完成 |
| **移动语义** | 资源所有权转移 = 指针窃取 + 源置空；O(1) 替代 O(n) 深拷贝 |
| **Scope/Ref 别名** | 统一引擎内存语义，`Scope`=独占，`Ref`=共享，比直接使用 `unique_ptr/shared_ptr` 更简洁 |
| **工厂模式 + 智能指针** | `CreateScope<T>(args...)` 封装 `make_unique`，配合完美转发支持任意构造参数 |
| **日志分层** | `CoreInfo` vs `Info` 区分引擎内部与客户端日志，宏封装简化调用 |

---

### 🏗️ 架构变化

```
之前（08-04）                     现在（08-05）
─────────────                    ─────────────
Application                      Application
 ├── m_Window (裸指针)             ├── m_Window (Scope<Window>)
 ├── LayerStack                    ├── LayerStack
 │    └── TestLayer                │    └── (空)
 └── main()                        └── Log::Init() + YUE_CORE_INFO

Window::Create()                 Window::Create()
 └── return new GLFWWindow        └── return CreateScope<GLFWWindow>

(无日志系统)                      Log
                                   ├── CoreInfo → [Yue Engine]
                                   └── Info     → [APP]

SandboxApp                       SandboxApp
 └── PushLayer(new TestLayer)      └── PushLayer(new SandboxLayer)
                                     └── YUE_INFO 验证日志
```

---

### ⚠️ 遇到的问题与解决

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| `CreateScope` 参数转发丢失值类别 | 未使用 `std::forward` | 添加 `std::forward<Args>(args)...` 完美转发 |
| SandboxLayer 中 `YUE_INFO` 找不到 | 未包含 Log.h | 在 SandboxLayer.cpp 中 `#include "Yue/Core/Log.h"` |
| `Scope<Window>` 赋值兼容性问题 | `Window::Create()` 仍返回裸指针 | 同步修改 Window.h/cpp 返回类型为 `Scope<Window>` |

---

### 📋 下一阶段计划

- [ ] 在 `Run()` 主循环中遍历调用各层 `OnUpdate()`
- [ ] 将 Input 事件（KeyPressed、MouseButtonPressed）接入 Layer 事件流
- [ ] 在 SandboxLayer 中接入 Input 查询，验证交互
- [ ] Renderer 模块初步搭建（Shader、Buffer）
- [ ] Log 系统扩展：Warn / Error 级别 + 时间戳 + 格式化输出
- [ ] 考虑 Overlay 的实际使用场景（如 ImGui 调试层）

---

## 2026-08-06（Timestep 时间步系统、断言系统与日志完善）

### 今日目标
引入 `Timestep` 时间步封装类，替代主循环中裸 `float` 传递 DeltaTime 的方式。  
搭建断言宏系统（`YUE_ASSERT` / `YUE_CORE_ASSERT`），为引擎提供开发阶段的契约验证能力。  
完善 Log 系统，新增 Error 级别日志输出。

---

### ✅ 完成内容

#### 1. Timestep 时间步封装

**新增：** `Timestep.h`

将原本以裸 `float` 传递的 DeltaTime 封装为独立类型：

```cpp
class Timestep
{
public:
    Timestep(float time = 0.0f) : m_Time(time) {}
    float GetSeconds() const { return m_Time; }
    float GetMilliseconds() const { return m_Time * 1000.0f; }
    operator float() const { return m_Time; }
private:
    float m_Time;
};
```

**设计要点：**

| 特性 | 说明 |
| :--- | :--- |
| `GetSeconds()` | 获取以秒为单位的时间步 |
| `GetMilliseconds()` | 获取以毫秒为单位的时间步 |
| `operator float()` | 隐式转换运算符，允许 `Timestep` 在需要 `float` 的上下文中自动转换 |

**为什么需要 Timestep 而不是直接传 float？**
- 语义明确：函数签名 `OnUpdate(Timestep ts)` 比 `OnUpdate(float dt)` 更清晰地表达"这是一个时间间隔"
- 单位安全：调用方可按需选择秒或毫秒，避免单位混淆
- 隐式转换：在数学运算中可直接当 `float` 使用，无需手动调用 `GetSeconds()`

---

#### 2. Application 主循环接入 Timestep

**修改：** `Application.h` / `Application.cpp`

新增成员：
```cpp
float m_LastFrameTime = 0.0f;
```

主循环中新增 DeltaTime 计算：
```cpp
void Application::Run() {
    while (m_Running) {
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

        float timeStep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        Timestep ts(timeStep);

        m_Window->Update();

        for (auto layer : m_LayerStack) {
            layer->OnUpdate(ts);
        }

        if (m_Window->ShouldClose()) {
            m_Running = false;
        }
    }
}
```

**关键变化：**
- 使用 `std::chrono::high_resolution_clock` 获取高精度时间
- 计算帧间时间差 `timeStep`
- 封装为 `Timestep` 对象后传入各层的 `OnUpdate(ts)`

---

#### 3. Layer 接口更新

**修改：** `Layer.h`

`OnUpdate()` 签名从无参数改为接收 `Timestep`：

```cpp
// 修改前
virtual void OnUpdate() {};

// 修改后
virtual void OnUpdate(Timestep ts) {};
```

新增头文件包含：
```cpp
#include "Timestep.h"
```

所有继承 `Layer` 的子类需同步更新 `OnUpdate` 签名。

---

### 4. Assert 断言系统

**新增：** `Assert.h`

```cpp
#ifdef YUE_ENABLE_ASSERT

#define YUE_ASSERT(x, ...) { if(!(x)) { YUE_ERROR(__VA_ARGS__); } }
#define YUE_CORE_ASSERT(x, message) { if(!(x)) { YUE_CORE_ERROR(message); __debugbreak(); } }

#else

#define YUE_ASSERT(x, ...)

#endif // YUE_ENABLE_ASSERT
```

**设计要点：**

| 宏 | 用途 | 行为 |
| :--- | :--- | :--- |
| `YUE_ASSERT(x, ...)` | 客户端断言 | 条件不满足时输出错误日志 |
| `YUE_CORE_ASSERT(x, message)` | 引擎核心断言 | 条件不满足时输出错误日志 + 触发调试器中断 |

**通过 `YUE_ENABLE_ASSERT` 预处理器宏控制：**
- Debug 构建：定义该宏 → 断言生效
- Release 构建：不定义 → 断言被完全移除，零运行时开销

**`__debugbreak()` 的作用：**
MSVC 专属 intrinsic，触发 `int 3` 指令，使调试器立即中断在断言位置，方便开发者查看调用栈和变量状态。

---

#### 5. Log 系统完善 — 新增 Error 级别

**修改：** `Log.h` / `Log.cpp`

新增函数：
```cpp
static void CoreError(const std::string& message);
static void Error(const std::string& message);
```

新增宏：
```cpp
#define YUE_CORE_ERROR(message) ::Yue::Log::CoreError(message)
#define YUE_ERROR(message) ::Yue::Log::Error(message)
```

实现：
```cpp
void Log::CoreError(const std::string& message) {
    std::cerr << "[Yue Engine Error] " << message << std::endl;
}

void Log::Error(const std::string& message) {
    std::cerr << "[APP Error] " << message << std::endl;
}
```

**当前 Log 系统完整接口：**

| 宏 | 前缀 | 输出流 | 用途 |
| :--- | :--- | :--- | :--- |
| `YUE_CORE_INFO` | `[Yue Engine]` | `std::cout` | 引擎内部信息 |
| `YUE_INFO` | `[APP]` | `std::cout` | 客户端信息 |
| `YUE_CORE_ERROR` | `[Yue Engine Error]` | `std::cerr` | 引擎内部错误 |
| `YUE_ERROR` | `[APP Error]` | `std::cerr` | 客户端错误 |

---

#### 6. SandboxLayer 同步更新

**修改：** `SandboxLayer.h` / `SandboxLayer.cpp`

`OnUpdate` 签名更新为接收 `Timestep`：
```cpp
void SandboxLayer::OnUpdate(Yue::Timestep ts) {
    YUE_INFO("DeltaTime:");
}
```

---

### 🧠 今日核心知识点总结

| 知识点 | 要点 |
| :--- | :--- |
| `operator float()` | 用户定义隐式类型转换运算符，允许对象在需要 `float` 时自动转换 |
| `std::chrono` | C++11 高精度时间库，`high_resolution_clock` 提供纳秒级精度 |
| `__debugbreak()` | MSVC intrinsic，触发 `int 3` 中断调试器，需包含 `<intrin.h>` |
| 断言 vs 错误处理 | 断言验证"不应发生的内部逻辑错误"，错误处理应对"可能发生的外部异常" |
| 预处理器条件编译 | `#ifdef YUE_ENABLE_ASSERT` 控制断言在 Release 中零开销移除 |

---

### 🏗️ 架构变化

```
之前（08-05）                     现在（08-06）
─────────────                    ─────────────
Application::Run()               Application::Run()
 ├── m_Window->Update()           ├── DeltaTime 计算（chrono）
 ├── layer->OnUpdate()            ├── Timestep ts(timeStep)
 └── (无时间步)                   ├── m_Window->Update()
                                  └── layer->OnUpdate(ts)

Layer                            Layer
 └── OnUpdate()                   └── OnUpdate(Timestep ts)

Log                              Log
 ├── CoreInfo                     ├── CoreInfo
 └── Info                         ├── Info
                                  ├── CoreError  ← 新增
                                  └── Error      ← 新增

(无断言系统)                      Assert
                                  ├── YUE_ASSERT
                                  └── YUE_CORE_ASSERT + __debugbreak()
```

---

### ⚠️ 遇到的问题与解决

| 问题 | 原因 | 解决方案 |
| :--- | :--- | :--- |
| `__debugbreak()` 无法识别 | 拼写错误（单下划线）或未包含头文件 | 确认使用双下划线 `__debugbreak()` 并包含 `<intrin.h>` |
| `OnUpdate` 签名不匹配 | `Layer.h` 更新后子类未同步 | 所有继承 Layer 的类同步修改 `OnUpdate(Timestep ts)` |
| Assert 宏中 `__VA_ARGS__` 为空时的处理 | `YUE_ASSERT(x)` 不传消息时 `__VA_ARGS__` 为空 | 当前设计下 `YUE_ASSERT` 要求必须传入消息参数 |

---

### 📋 下一阶段计划

- [ ] Log 系统升级：引入 spdlog 替代手动 `std::cout`，支持格式化输出 `{0}`、`{1}` 占位符
- [ ] Assert 系统完善：区分带消息/无消息版本，无消息版本自动输出断言表达式、文件名、行号
- [ ] 将 Input 事件（KeyPressed、MouseButtonPressed）接入 Layer 事件流
- [ ] Renderer 模块初步搭建（Shader、Buffer）
- [ ] 考虑 Overlay 的实际使用场景（如 ImGui 调试层）

---

## 2026-08-07 开发日志（Renderer 模块 — GraphicsContext 抽象与 OpenGL 实现）

### 🎯 今日目标

搭建 Renderer 模块的基础架构，完成 `GraphicsContext` 抽象接口与 `OpenGLContext` 具体实现。  
将原本散落在 `GLFWWindow` 中的 OpenGL 上下文管理逻辑抽离为独立的渲染上下文层，实现 **窗口系统与渲染API的解耦**。  
引入 `OpenGL.h` 统一头文件封装 GLFW/GLAD 依赖，建立编译防火墙模式。

---

### ✅ 完成内容

#### 1. GraphicsContext 抽象接口

**新增：** `Renderer/GraphicsContext.h` / `GraphicsContext.cpp`

```cpp
class GraphicsContext
{
public:
    virtual ~GraphicsContext() = default;
    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;
    static Scope<GraphicsContext> Create(void* window);
};
```

| 设计决策 | 说明 |
|----------|------|
| 纯虚函数 `Init()` / `SwapBuffers()` | 定义渲染上下文的契约，子类必须实现 |
| `void* window` 参数 | 避免基类头文件依赖任何窗口库类型，保持平台无关性 |
| 静态工厂 `Create()` | 隐藏具体实现类，调用方只持有 `Scope<GraphicsContext>` |

`GraphicsContext.cpp` 中工厂实现：
```cpp
Scope<GraphicsContext> GraphicsContext::Create(void* window) {
    return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
}
```

> ⚠️ 当前工厂硬编码返回 `OpenGLContext`。未来支持多后端时，可通过宏或运行时配置切换。

#### 2. OpenGLContext 具体实现

**新增：** `Platform/OpenGL/OpenGLContext.h` / `OpenGLContext.cpp`

##### 头文件 — 编译防火墙设计

```cpp
#pragma once
#include "Renderer/GraphicsContext.h"

struct GLFWwindow;  // ← 前向声明，不 #include <GLFW/glfw3.h>

namespace Yue {
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);
        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow* m_WindowHandle;
    };
}
```

**为什么用 `struct GLFWwindow;` 而不是 `#include <GLFW/glfw3.h>`？**

- `glfw3.h` 是重量级头文件，会传递引入 Windows.h / X11.h 等平台头文件
- 头文件被引擎数十个文件包含，污染会导致编译时间剧增
- `OpenGLContext.h` 仅使用 `GLFWwindow*`（指针），前向声明即可满足编译需求
- 使用 `struct` 而非 `class` 是因为 GLFW 是 C 库，原始类型为 `struct _GLFWwindow`

##### 实现文件 — 完整依赖在此引入

```cpp
#include "OpenGLContext.h"
#include "Core/Assert.h"
#include "OpenGL.h"  // ← 统一封装 GLFW + GLAD

void OpenGLContext::Init() {
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    YUE_CORE_ASSERT(status, "Failed to initialize Glad!");
    YUE_CORE_INFO("OpenGL Context Initialized");
}

void OpenGLContext::SwapBuffers() {
    glfwSwapBuffers(m_WindowHandle);
}
```

#### 3. OpenGL.h 统一封装头文件

**新增：** `Platform/OpenGL/OpenGL.h`

```cpp
#pragma once
#define GLFW_INCLUDE_NONE   // ← 阻止 glfw3.h 自动引入 OpenGL 头文件
#include <GLFW/glfw3.h>
#include <glad/glad.h>
```

| 要点 | 说明 |
|------|------|
| `GLFW_INCLUDE_NONE` | GLFW 默认会引入系统 OpenGL 头文件，与 GLAD 冲突；此宏阻止该行为 |
| 先 GLFW 后 GLAD | GLAD 需要 GLFW 的类型定义，顺序不可颠倒 |
| 单一入口 | 所有 OpenGL 相关 `.cpp` 只需 `#include "OpenGL.h"`，无需重复管理两个库的包含顺序 |

#### 4. 关键知识点梳理

| 问题 | 解答 |
|------|------|
| `OpenGLContext` 是抽象类吗？ | **不是**。它实现了所有纯虚函数（无 `= 0`），是可实例化的具体类 |
| `virtual ... override` 的含义？ | `override` 是编译器检查标记，确保签名匹配基类虚函数；`virtual` 可选但推荐保留以提高可读性 |
| `.h` 前向声明 vs `.cpp` include 是同一个类型吗？ | **是同一个**。C++ 标准保证同一作用域内前向声明与完整定义指向同一实体 |
| 为什么 `.cpp` 还要 include glfw3.h？ | 因为实现确实依赖 GLFW API。关键是**只有这一个翻译单元**需要解析它，不影响引擎其余代码 |

---

### 🏗️ 架构变化

```
之前（08-06）                          现在（08-07）
─────────────                          ─────────────
GLFWWindow                             GLFWWindow
 ├── glfwMakeContextCurrent()           ├── (不再直接管理 GL 上下文)
 ├── gladLoadGLLoader()                 └── 持有 Scope<GraphicsContext>
 └── glfwSwapBuffers()                        ↓
                                       GraphicsContext (纯抽象)
(窗口与渲染API耦合)                     ├── Init() = 0
                                       ├── SwapBuffers() = 0
                                       └── Create(void*) → 工厂

                                       OpenGLContext (具体实现)
                                        ├── glfwMakeContextCurrent()
                                        ├── gladLoadGLLoader()
                                        └── glfwSwapBuffers()

                                       OpenGL.h (编译防火墙)
                                        ├── #define GLFW_INCLUDE_NONE
                                        ├── <GLFW/glfw3.h>
                                        └── <glad/glad.h>
```

**依赖方向：**
```
Application → Window → GraphicsContext (抽象)
                            ↑
                      OpenGLContext (Platform层)
                            ↓
                      OpenGL.h → GLFW + GLAD
```

窗口系统不再直接调用 OpenGL/GLFW 渲染函数，而是通过 `GraphicsContext` 抽象接口间接操作。未来替换渲染后端（Vulkan、DirectX）只需新增对应的 Context 实现，窗口代码零改动。

---

### ⚠️ 遇到的问题与解决

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| `gladLoadGLLoader` 编译失败 | 未设置 `GLFW_INCLUDE_NONE`，GLFW 自带的 GL 头文件与 GLAD 冲突 | 在 `OpenGL.h` 中添加 `#define GLFW_INCLUDE_NONE` |
| `OpenGLContext.h` 中写 `class GLFWwindow;` 产生警告 | GLFW 是 C 库，原始类型为 struct | 改为 `struct GLFWwindow;` |
| 工厂函数 `Create(void*)` 需要 `static_cast` | `void*` 是无类型指针，无法隐式转换为 `GLFWwindow*` | 在工厂实现中显式 `static_cast<GLFWwindow*>(window)` |

---

### 📋 下一阶段计划

- [ ] 将 `GraphicsContext` 接入 `GLFWWindow`，替换原有的直接 GLFW 调用
- [ ] 在 `OpenGLContext::Init()` 中打印 GPU Vendor / Renderer / Version 信息
- [ ] 添加 OpenGL 版本断言（要求 ≥ 4.5）
- [ ] Shader 类初步设计（编译、链接、Uniform 设置）
- [ ] VertexBuffer / IndexBuffer 抽象接口
- [ ] 考虑 `GraphicsContext` 工厂的多后端选择机制（宏 / 运行时配置）

---

## 2026-08-10 (Hazel 引擎渲染系统重构)

### 🎯 今日目标
完成 Hazel 引擎渲染系统的分层架构搭建，实现 OpenGL 后端抽象与高级渲染器接口，为后续批渲染优化和编辑器功能奠定基础。

---

### ✅ 已完成工作

#### 1. 渲染 API 抽象层 (`RendererAPI`)
- **新增** `RendererAPI.h / .cpp`：定义纯虚渲染接口，解耦上层逻辑与具体图形 API
  - 封装核心渲染原语：`Init`, `Clear`, `SetViewport`, `DrawIndexed`, `DrawLines` 等
  - 实现工厂模式 `Create()` + 静态 API 枚举，支持运行时后端切换
  - 使用 `Scope<>` 管理生命周期，零裸指针
- **新增** `OpenGLRendererAPI.h / .cpp`：OpenGL 4.x 后端完整实现
  - 初始化配置：Debug Output（同步回调+通知过滤）、Alpha Blending、Depth Test、Line Smooth
  - 所有绘制命令通过 GLEW/GLAD 安全调用，含断言校验

#### 2. 高级渲染器 (`Renderer`)
- **新增** `Renderer.h / .cpp`：场景级渲染管理器
  - 实现 `BeginScene/EndScene` 作用域模式，绑定 OrthographicCamera VP 矩阵
  - 统一 `Submit(Shader, VAO, Transform)` 接口，隐藏 Bind/Upload/Draw 细节
  - 内部通过 `RenderCommand` 转发至 `RendererAPI`，自身不含任何 GL 代码
  - `SceneData` 结构体集中管理全局渲染状态，预留扩展点

#### 3. 沙盒测试层 (`SandboxLayer`)
- **更新** `SandboxLayer.h`：适配新渲染器接口
  - 移除直接 OpenGL 调用，改用 `Renderer::Submit()` 提交绘制
  - 验证 BeginScene/EndScene 流程正确性
  - 测试正交相机下的 2D 图元渲染

### 4. 文档
- **新增** `Development.md`：记录架构设计决策与待办事项

---

### 🏗️ 架构变更

```
[Before] Application → 直接调用 gl* 函数
[After]  Application → Renderer → RenderCommand → RendererAPI → OpenGLRendererAPI
```

| 层级 | 文件 | 职责 |
|------|------|------|
| 高层 | SandboxLayer | 游戏/编辑器逻辑，仅调用 Submit |
| 中层 | Renderer | 场景管理、状态打包、批渲染入口 |
| 命令层 | RenderCommand | 静态转发，线程安全预留 |
| 抽象层 | RendererAPI | 纯虚接口 + 工厂 |
| 实现层 | OpenGLRendererAPI | OpenGL 具体实现 |

---

### 🔧 关键技术决策

1. **OrthographicCamera Only**：当前 Renderer 仅支持正交投影，优先保障 2D 编辑器管线；透视相机支持列入下一阶段
2. **Debug Output 条件编译**：`GL_DEBUG_OUTPUT_SYNCHRONOUS` 仅在 `HZ_DEBUG` 下启用，Release 零开销
3. **Submit 默认 Transform**：`glm::mat4(1.0f)` 默认值简化无变换物体的调用语法
4. **SceneData 堆分配**：使用 `Scope<SceneData>` 而非静态实例，避免头文件中暴露 glm 完整定义

---

### ⚠️ 已知问题 & 限制

- [ ] `Renderer::Submit` 尚未实现批渲染合并，每次调用产生独立 DrawCall
- [ ] 缺少 Texture 绑定抽象，当前 Shader 需手动 Bind Texture
- [ ] `GL_LINE_SMOOTH` 为遗留特性，部分现代驱动可能忽略
- [ ] 未实现多 Viewport / 多 Camera 渲染通道
- [ ] RendererAPI::Create() 硬编码 OpenGL，缺少配置文件/宏切换机制

---

## 2026-08-14 开发日志（Renderer 模块 — Buffer / VertexArray 抽象与 OpenGL 实现）

### 🎯 今日目标

完成渲染管线中 **Buffer** 与 **VertexArray** 的完整抽象层设计及 OpenGL 后端实现。  
建立 `ShaderDataType` → `BufferLayout` → `VertexBuffer` → `VertexArray` 的数据描述链路，使上层代码无需接触任何 OpenGL API 即可完成顶点数据的声明、上传与绑定。  
在 SandboxLayer 中验证整条链路的端到端可用性。

---

### ✅ 完成内容

#### 1. ShaderDataType 与 BufferLayout 数据描述体系

**新增 / 修改：** `Buffer.h`

定义了引擎统一的顶点数据类型枚举及大小计算函数：

```cpp
enum class ShaderDataType {
    None = 0, Float, Float2, Float3, Float4,
    Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type);
```

| 类型 | 大小 (bytes) | 分量数 |
|------|-------------|--------|
| Float | 4 | 1 |
| Float2 | 8 | 2 |
| Float3 | 12 | 3 |
| Float4 | 16 | 4 |
| Mat3 | 36 | 3 |
| Mat4 | 64 | 4 |
| Int / Int2 / Int3 / Int4 | 4~16 | 1~4 |
| Bool | 1 | 1 |

**BufferElement** 结构体封装单个顶点属性的元信息：

```cpp
struct BufferElement {
    std::string Name;
    ShaderDataType Type;
    uint32_t Size;
    size_t Offset;
    bool Normalized;

    BufferElement(ShaderDataType type, const std::string& name, bool normalized = false);
    uint32_t GetComponentCount() const;
};
```

**BufferLayout** 类接收 `std::initializer_list<BufferElement>`，自动计算每个元素的偏移量和总步长：

```cpp
class BufferLayout {
public:
    BufferLayout(std::initializer_list<BufferElement> elements);
    uint32_t GetStride() const;
    const std::vector<BufferElement>& GetElements() const;
    // 提供 begin()/end() 迭代器，支持 range-for 遍历
private:
    void CaculateOffsetsAndStride();
};
```

**关键设计决策：**

- `initializer_list` 构造函数允许 `{ {Float3, "a_Position"}, {Float2, "a_TexCoord"} }` 的声明式语法，无需手动计算 offset/stride
- `Mat3/Mat4` 的 `GetComponentCount()` 返回列数（3/4），而非总元素数，因为 OpenGL 中矩阵作为顶点属性时按列拆分为多个 location
- 提供非 const 和 const 两套迭代器接口，兼容 `OpenGLVertexArray::AddVertexBuffer` 中的 range-for 遍历

#### 2. VertexBuffer / IndexBuffer 抽象接口

**新增 / 修改：** `Buffer.h`

```cpp
class VertexBuffer {
public:
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void SetData(const void* data, uint32_t size) = 0;
    virtual const BufferLayout& GetLayout() const = 0;
    virtual void SetLayout(const BufferLayout& layout) = 0;

    static Ref<VertexBuffer> Create(uint32_t size);
    static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
};

class IndexBuffer {
public:
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual uint32_t GetCount() const = 0;

    static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
};
```

| 设计要点 | 说明 |
|---------|------|
| 双重载 Create | 静态创建（预分配 GPU 内存）+ 数据创建（一次性上传），覆盖初始化和动态更新两种场景 |
| SetLayout 分离 | Buffer 只负责存储数据和布局描述，不直接调用 GL 函数；布局到 VAO 的绑定由 VertexArray 完成 |
| Ref<> 管理生命周期 | 所有 Buffer 通过共享指针持有，避免手动 delete |

#### 3. OpenGLVertexBuffer / OpenGLIndexBuffer 实现

**新增：** `Platform/OpenGL/OpenGLBuffer.h` / `OpenGLBuffer.cpp`

**OpenGLVertexBuffer：**

```cpp
// 静态创建：预分配指定大小的 GPU 内存
OpenGLVertexBuffer(uint32_t size) {
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

// 数据创建：一次性上传顶点数据
OpenGLVertexBuffer(float* vertices, uint32_t size) {
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

// 动态更新
void SetData(const void* data, uint32_t size) {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}
```

**OpenGLIndexBuffer：**

```cpp
OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count) {
    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}
```

**关键技术点：**

- 使用 `glCreateBuffers`（DSA 风格）而非 `glGenBuffers` + `glBindBuffer`，减少不必要的状态绑定
- 静态创建使用 `GL_DYNAMIC_DRAW`，为后续 `SetData` 动态更新预留性能提示
- 数据创建使用 `GL_STATIC_DRAW`，适合不会频繁修改的网格数据

#### 4. VertexArray 抽象接口与工厂

**新增：** `Renderer/VertexArray.h` / `VertexArray.cpp`

```cpp
class VertexArray {
public:
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexbuffer) = 0;
    virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexbuffer) = 0;
    virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
    virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

    static Ref<VertexArray> Create();
};
```

工厂实现根据当前 RendererAPI 选择后端：

```cpp
Ref<VertexArray> VertexArray::Create() {
    switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:   return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
    }
    return nullptr;
}
```

#### 5. OpenGLVertexArray 实现 — 核心难点

**新增：** `Platform/OpenGL/OpenGLVertexArray.h` / `OpenGLVertexArray.cpp`

这是今日最复杂的部分，负责将抽象的 `BufferLayout` 翻译为具体的 OpenGL 顶点属性配置：

```cpp
void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout) {
        switch (element.Type) {
            // 基础浮点类型
            case ShaderDataType::Float: case ShaderDataType::Float2:
            case ShaderDataType::Float3: case ShaderDataType::Float4:
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset);
                m_VertexBufferIndex++;
                break;

            // 整数类型（使用 I 系列 API，不做归一化）
            case ShaderDataType::Int: case ShaderDataType::Int2:
            case ShaderDataType::Int3: case ShaderDataType::Int4:
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribIPointer(m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    layout.GetStride(),
                    (const void*)element.Offset);
                m_VertexBufferIndex++;
                break;

            // 矩阵类型：按列拆分，每列占用一个独立的 attribute location
            case ShaderDataType::Mat3: case ShaderDataType::Mat4: {
                uint8_t count = element.GetComponentCount(); // Mat4=4列, Mat3=3列
                for (int i = 0; i < count; i++) {
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(m_VertexBufferIndex,
                        count,  // 每列的分量数（Mat4每列4个float）
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * count * i));
                    m_VertexBufferIndex++;
                }
                break;
            }
        }
    }
    m_VertexBuffers.push_back(vertexBuffer);
}
```

**矩阵属性的特殊处理原理：**

GLSL 中 `mat4` 等价于 `vec4[4]`，占据 **4 个连续的 attribute location**。因此不能像 `Float3` 那样一次 `glVertexAttribPointer` 搞定，必须循环 4 次，每次配置一列：

| 循环轮次 | location | offset 偏移 | 含义 |
|---------|----------|------------|------|
| i=0 | base+0 | +0 bytes | column 0 |
| i=1 | base+1 | +16 bytes | column 1 |
| i=2 | base+2 | +32 bytes | column 2 |
| i=3 | base+3 | +48 bytes | column 3 |

`m_VertexBufferIndex` 是跨所有 VertexBuffer 的全局递增计数器，确保多个 VBO 添加到同一个 VAO 时 location 不冲突。

**辅助函数：**

```cpp
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
    // Float/Float2/Float3/Float4/Mat3/Mat4 → GL_FLOAT
    // Int/Int2/Int3/Int4                     → GL_INT
    // Bool                                   → GL_BOOL
}
```

#### 6. SandboxLayer 端到端验证

**修改：** `SandboxLayer.h` / `SandboxLayer.cpp`

```cpp
void SandboxLayer::OnAttach() {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    m_VertexBuffer = Yue::VertexBuffer::Create(vertices, sizeof(vertices));
    m_VertexBuffer->SetLayout({ {Yue::ShaderDataType::Float3, "a_Position"} });

    m_VertexArray = Yue::VertexArray::Create();
    m_VertexArray->AddVertexBuffer(m_VertexBuffer);
}

void SandboxLayer::OnUpdate(Yue::Timestep ts) {
    Yue::Renderer::SetClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    Yue::Renderer::Clear();
}
```

验证了完整的声明式数据流：

```
vertices[] → VertexBuffer::Create() → SetLayout({...}) → VertexArray::AddVertexBuffer()
                                                              ↓
                                                    OpenGL glVertexAttribPointer
```

上层代码零 OpenGL 调用，仅通过引擎抽象接口完成三角形顶点数据的声明与绑定。

---

### 🧠 今日核心知识点总结

| 知识点 | 要点 |
|--------|------|
| `std::initializer_list` | 允许 `{ {...}, {...} }` 语法构造容器，编译器自动推导元素类型并调用对应构造函数 |
| BufferLayout 自动偏移计算 | 构造函数中遍历元素累加 offset/stride，用户只需声明类型和名称 |
| 矩阵顶点属性的列拆分 | mat4 = 4 个 vec4 列，每列独立 location，需循环配置 `glVertexAttribPointer` |
| `glVertexAttribPointer` vs `glVertexAttribIPointer` | 浮点/归一化整数用前者，纯整数用后者（I 系列不做隐式浮点转换） |
| DSA 风格 `glCreateBuffers` | 创建即生成 ID，无需先 bind 再操作，减少全局状态污染 |
| `m_VertexBufferIndex` 全局递增 | 跨多个 VBO 的 location 不冲突，VAO 内统一管理属性索引 |
| `Ref<>` 持有 Buffer | VertexArray 通过 shared_ptr 持有 VBO/IBO，防止 Buffer 提前释放导致 VAO 引用悬空 |

---

### 🏗️ 架构变化

```
之前（08-10）                              现在（08-14）
─────────────                              ─────────────
Renderer                                   Renderer
 ├── RendererAPI                            ├── RendererAPI
 ├── RenderCommand                          ├── RenderCommand
 └── SceneData                              └── SceneData

(无 Buffer/VAO 抽象)                       Buffer.h
                                            ├── ShaderDataType + ShaderDataTypeSize()
                                            ├── BufferElement + BufferLayout
                                            ├── VertexBuffer (抽象 + 工厂)
                                            └── IndexBuffer  (抽象 + 工厂)

                                           Platform/OpenGL/OpenGLBuffer.h/.cpp
                                            ├── OpenGLVertexBuffer
                                            └── OpenGLIndexBuffer

                                           VertexArray.h/.cpp
                                            ├── VertexArray (抽象 + 工厂)
                                            └── Create() → RendererAPI 分发

                                           Platform/OpenGL/OpenGLVertexArray.h/.cpp
                                            ├── AddVertexBuffer() → 解析 BufferLayout
                                            ├── 矩阵列拆分逻辑
                                            └── SetIndexBuffer()

                                           SandboxLayer
                                            ├── VertexBuffer::Create() + SetLayout()
                                            └── VertexArray::Create() + AddVertexBuffer()
```

**数据流路径：**

```
ShaderDataType → BufferElement → BufferLayout → VertexBuffer.SetLayout()
                                                        ↓
                                              VertexArray.AddVertexBuffer()
                                                        ↓
                                              OpenGLVertexArray 解析 layout
                                                        ↓
                                              glEnableVertexAttribArray
                                              glVertexAttribPointer / IPointer
                                              (矩阵: 循环 N 列)
                                                        ↓
                                              VAO 绑定完成，可绘制
```

---

### ⚠️ 已知问题与待修复

| 问题 | 说明 | 优先级 |
|------|------|--------|
| `OpenGLVertexArray` 析构使用 `glDeleteBuffers` | 应使用 `glDeleteVertexArrays`，当前误删 buffer 而非 VAO | 🔴 高 |
| Mat4 列偏移计算 `sizeof(float) * count * i` | `count` 是列数（4），但每列大小应为 `sizeof(float) * count`，当前公式正确但变量命名易混淆，建议改为 `columnSize` | 🟡 中 |
| `CaculateOffsetsAndStride` 拼写错误 | 应为 `Calculate`，不影响功能但影响可读性 | 🟢 低 |
| 缺少 Instancing 支持 | `AddVertexBuffer` 未暴露 `divisor` 参数，无法配置 per-instance 属性 | 📋 下阶段 |
| 缺少 VertexBuffer 动态 resize | `SetData` 使用 `glBufferSubData`，超出原始分配大小时行为未定义 | 📋 下阶段 |

---

## 2026-08-17 开发日志（Renderer 模块 — Shader 抽象与 OpenGL 实现、RenderCommand 静态门面完善）

### 🎯 今日目标

完成渲染管线中 **Shader** 的完整抽象层设计及 OpenGL 后端实现。  
建立 `Shader` 抽象接口 → `OpenGLShader` 具体实现的完整链路，使上层代码无需接触任何 OpenGL Shader API 即可完成着色器的编译、链接、绑定与 Uniform 设置。  
完善 `RenderCommand` 静态门面类，补全 `DrawIndexed` 等绘制指令的转发机制。  
修复 SandboxLayer 中 `Renderer::Submit` 的链接错误，验证 Shader + VAO + RenderCommand 整条渲染链路的端到端可用性。

---

### ✅ 完成内容

#### 1. Shader 抽象接口

**新增 / 修改：** `Renderer/Shader.h`

定义了引擎统一的着色器抽象基类：

```cpp
class Shader {
public:
    virtual ~Shader() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    static Ref<Shader> Create(const std::string& name,
                              const std::string& vertexSrc,
                              const std::string& fragmentSrc);
};
```

| 设计要点 | 说明 |
|---------|------|
| `Bind()` / `Unbind()` 纯虚函数 | 定义着色器激活/解绑契约，子类必须实现 |
| 静态工厂 `Create()` | 隐藏具体实现类，调用方只持有 `Ref<Shader>` |
| 参数为源码路径而非内联字符串 | 支持从文件加载 GLSL 源码，便于外部编辑和热重载 |

#### 2. OpenGLShader 具体实现

**新增：** `Platform/OpenGL/OpenGLShader.h` / `OpenGLShader.cpp`

##### 核心流程

```
读取顶点/片元源码文件
    ↓
CompileShader(GL_VERTEX_SHADER, vertexSource)
CompileShader(GL_FRAGMENT_SHADER, fragmentSource)
    ↓
CreateProgram(vertexShader, fragmentShader)
    ├── glAttachShader × 2
    ├── glLinkProgram
    └── glDeleteShader × 2（链接后释放中间对象）
    ↓
m_RendererID = program handle
```

##### 关键实现细节

| 方法 | 职责 | 技术要点 |
|------|------|----------|
| `ReadFile()` | 从磁盘读取 GLSL 源码 | `std::ifstream` + `stringstream`，返回完整字符串 |
| `CompileShader()` | 编译单个着色器阶段 | `glCreateShader` → `glShaderSource` → `glCompileShader` |
| `CreateProgram()` | 链接着色器程序 | `glCreateProgram` → `glAttachShader` × 2 → `glLinkProgram` → `glDeleteShader` × 2 |
| `Bind()` | 激活着色器程序 | `glUseProgram(m_RendererID)` |
| `Unbind()` | 解绑着色器程序 | `glUseProgram(0)` |
| 析构函数 | 释放 GPU 资源 | `glDeleteProgram(m_RendererID)` |

##### 编译防火墙

`OpenGLShader.h` 中使用 `typedef unsigned int GLenum;` 前向声明 GL 类型，避免头文件直接依赖 `<glad/glad.h>`。完整 OpenGL 头文件仅在 `.cpp` 中通过 `#include "OpenGL.h"` 引入。

#### 3. RenderCommand 静态门面完善

**修改：** `RenderCommand.h` / `RenderCommand.cpp`

`RenderCommand` 作为静态门面（Facade），将所有渲染指令转发至底层 `RendererAPI`：

```cpp
static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) {
    s_RendererAPI->DrawIndexed(vertexArray, indexCount);
}
```

**静态成员定义：**

```cpp
// RenderCommand.cpp
Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
```

| 知识点 | 说明 |
|--------|------|
| 类内声明 vs 类外定义 | 类内 `static Scope<RendererAPI> s_RendererAPI;` 仅为声明；类外必须重复完整类型进行定义+初始化 |
| 多态持有 | `s_RendererAPI` 类型为 `Scope<RendererAPI>`（抽象基类指针），实际指向 `OpenGLRendererAPI` 具体子类实例 |
| 自包含设计 | `DrawIndexed` 内部调用 `vertexArray->Bind()`，保证无论调用路径如何都能正确工作 |

#### 4. Renderer::Submit 实现与冗余绑定分析

**修改：** `Renderer.h` / `Renderer.cpp`

```cpp
void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray) {
    shader->Bind();
    vertexArray->Bind();           // ← 第1次绑定
    RenderCommand::DrawIndexed(vertexArray); // ← 内部再次绑定（冗余）
}
```

**冗余绑定分析：**

| 绑定位置 | 原因 | 是否必要 |
|----------|------|----------|
| `Renderer::Submit` | 上层编排逻辑，确保 VAO 在 DrawCall 前就绪 | ⚠️ 冗余 |
| `OpenGLRendererAPI::DrawIndexed` | 底层自包含设计，保证独立调用时也能正确工作 | ✅ 必要 |

**决策：** 当前保留双重绑定以确保安全性。未来优化时可移除 `Submit` 中的 `vertexArray->Bind()`，让底层 API 独自负责绑定。重复绑定同一 VAO 对现代驱动性能影响极小（状态缓存去重）。

#### 5. SandboxLayer 链接错误修复

**问题：** `LNK2019: 无法解析的外部符号 Yue::Renderer::Submit`

**原因：** `Renderer::Submit` 在头文件中声明但 `.cpp` 中未实现（或签名不匹配）。

**解决：** 在 `Renderer.cpp` 中补充完整实现，确保命名空间、参数类型与头文件声明完全一致。

#### 6. C4819 编码警告处理

构建日志中出现大量 `warning C4819`，提示源文件包含非当前代码页（936）字符。

**解决方案：** 将所有 `.h/.cpp` 文件保存为 **UTF-8 with BOM** 格式，消除编码警告，避免潜在的源码误解析问题。

---

### 🧠 今日核心知识点总结

| 知识点 | 要点 |
|--------|------|
| Shader 编译链接流程 | Compile → Attach → Link → DeleteShader，Program 是唯一需要长期持有的句柄 |
| 编译防火墙 | `.h` 中前向声明 GL 类型，`.cpp` 中才引入完整 OpenGL 头文件，减少编译依赖传播 |
| 静态成员类外定义 | C++ 要求类外定义时必须重复完整类型，这是独立翻译单元级语句的语法要求 |
| 抽象类指针持有具体对象 | `Scope<RendererAPI>` 可指向 `OpenGLRendererAPI`，通过虚函数表实现运行时多态 |
| 门面模式（Facade） | `RenderCommand` 将多个底层操作封装为简洁的静态接口，上层无需感知 `RendererAPI` 细节 |
| UTF-8 with BOM | MSVC 编译器识别 UTF-8 编码的标志，缺少 BOM 时回退到系统代码页导致 C4819 |

---

### 🏗️ 架构变化

```
之前（08-14）                     现在（08-17）
─────────────                     ─────────────
Renderer                          Renderer
├── RendererAPI                   ├── RendererAPI
├── RenderCommand                 ├── RenderCommand
│   └── (仅 Clear/SetClearColor)  │   ├── DrawIndexed ← 新增
└── SceneData                     │   └── s_RendererAPI (类外定义)
                                  ├── SceneData
Buffer / VertexArray              └── Submit(Shader, VAO) ← 新增
(已完成)
                                  Shader (抽象) ← 新增
                                  ├── Bind() / Unbind()
                                  └── Create() → 工厂

                                  Platform/OpenGL/OpenGLShader ← 新增
                                  ├── ReadFile()
                                  ├── CompileShader()
                                  ├── CreateProgram()
                                  ├── Bind() / Unbind()
                                  └── ~OpenGLShader() → glDeleteProgram

SandboxLayer                      SandboxLayer
└── VBO + VAO 创建                ├── VBO + VAO 创建
                                  ├── Shader::Create() ← 新增
                                  └── Renderer::Submit() ← 新增
```

**完整渲染数据流：**

```
Shader::Create(name, vertPath, fragPath)
    ↓ OpenGLShader: ReadFile → Compile → Link → Program ID
    ↓
Renderer::Submit(shader, vertexArray)
    ├── shader->Bind()         → glUseProgram
    ├── vertexArray->Bind()    → glBindVertexArray
    └── RenderCommand::DrawIndexed(vertexArray)
        ├── vertexArray->Bind()  → glBindVertexArray (冗余但安全)
        └── glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr)
```

---

### ⚠️ 已知问题与待修复

| 问题 | 说明 | 优先级 |
|------|------|--------|
| `OpenGLVertexArray` 析构使用 `glDeleteBuffers` | 应使用 `glDeleteVertexArrays`，当前误删 buffer 而非 VAO | 🔴 高 |
| `CaculateOffsetsAndStride` 拼写错误 | 应为 `Calculate`，不影响功能但影响可读性 | 🟢 低 |
| Shader 缺少 Uniform 设置接口 | `SetMat4`/`SetFloat` 等方法尚未实现，`Renderer::Submit` 中无法上传 VP 矩阵 | 🔴 高 |
| Shader 缺少编译/链接错误日志 | 当前编译失败时无任何输出，调试困难 | 🟡 中 |
| `Renderer::Submit` 缺少 Transform 参数 | 头文件注释 TODO 提到需补充位移矩阵，当前版本未实现 | 🟡 中 |
| 缺少 Texture 绑定抽象 | Shader 需手动 Bind Texture，未纳入 Submit 流程 | 📋 下阶段 |

---

### 📋 下一阶段计划

- [ ] 实现 Shader Uniform 设置接口（`SetMat4`, `SetFloat`, `SetInt` 等）
- [ ] 添加 Shader 编译/链接错误日志输出（`glGetShaderInfoLog` / `glGetProgramInfoLog`）
- [ ] 修复 `OpenGLVertexArray` 析构函数中的 `glDeleteBuffers` → `glDeleteVertexArrays`
- [ ] 在 `Renderer::Submit` 中补充 Transform 矩阵参数并上传至 Shader
- [ ] 在 SandboxLayer 中完成带颜色的三角形实际绘制验证
- [ ] 添加 IndexBuffer 到 SandboxLayer 验证索引绘制
- [ ] Texture 抽象接口初步设计

---

## 2026-08-19 开发日志（Renderer 模块 — Shader Uniform 接口、SceneCamera 透视投影与 Sandbox 渲染验证）

### 🎯 今日目标

完成 Shader Uniform 设置接口（`SetMat4`）的抽象与 OpenGL 实现，使上层代码能够向 GPU 上传变换矩阵。
设计并实现 `Camera` 基类与 `SceneCamera` 派生类，建立引擎统一的相机投影体系。
引入 `TransformComponent` 与 `CameraComponent` 数据结构，为未来 ECS 架构预布局。
在 SandboxLayer 中完成 **Shader + VAO + Transform + Camera + View** 整条渲染链路的端到端验证，成功绘制带透视投影的三角形。

### ✅ 完成内容

#### 1. Shader Uniform 接口扩展

**修改：** `Renderer/Shader.h`

新增纯虚函数：

```cpp
virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;
```

| 设计要点 | 说明 |
|---|---|
| 参数为 `const std::string&` | 避免 uniform 名称字符串拷贝开销 |
| 参数为 `const glm::mat4&` | 矩阵按引用传递，避免 64 bytes 拷贝 |
| 纯虚函数 | 强制所有后端实现 Uniform 上传能力 |

#### 2. OpenGLShader Uniform 实现

**修改：** `Platform/OpenGL/OpenGLShader.h` / `OpenGLShader.cpp`

```cpp
void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix) {
    GLuint location = glGetUniformLocation(m_RendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
```

| 技术要点 | 说明 |
|---|---|
| `glGetUniformLocation` | 按名称查询 uniform 位置，未找到时返回 -1，`glUniformMatrix4fv` 对 -1 静默忽略 |
| `GL_FALSE` | 表示矩阵以列主序（column-major）传入，与 GLM 默认存储一致，无需转置 |
| `glm::value_ptr` | 获取矩阵底层 float* 指针，兼容 OpenGL C API |
| 编译防火墙 | `.h` 中仅前向声明 `typedef unsigned int GLenum;`，完整 GL 类型在 `.cpp` 中通过 `OpenGL.h` 引入 |

#### 3. Camera 基类设计

**新增：** `Renderer/Camera.h`

```cpp
class Camera {
public:
    Camera() = default;
    Camera(const glm::mat4& projection) : m_Projection(projection) {}
    virtual ~Camera() = default;
    const glm::mat4& GetProjection() const { return m_Projection; }
protected:
    glm::mat4 m_Projection = glm::mat4(1.0f);
};
```

| 设计决策 | 说明 |
|---|---|
| `m_Projection` 放在 protected | 允许派生类直接写入投影矩阵，外部只读 |
| `GetProjection()` 返回 `const&` | 避免每帧拷贝 64 bytes 矩阵 |
| 虚析构函数 | 支持通过基类指针安全删除派生类对象 |
| 不含 View 矩阵 | Camera 只负责投影；View 由外部 Transform 或 lookAt 计算，保持单一职责 |

#### 4. SceneCamera 透视投影实现

**新增：** `SceneCamera.h` / `SceneCamera.cpp`

```cpp
class SceneCamera : public Camera {
public:
    SceneCamera();
    void SetViewportSize(uint32_t width, uint32_t height);
    void SetPerspective(float verticalFOV, float nearClip, float farClip);
private:
    void RecalculateViewMatrix();  // 实际重算的是 Projection
    float m_PerspectiveFOV = glm::radians(45.0f);
    float m_PerspectiveNear = 0.1f, m_PerspectiveFar = 100.0f;
    float m_AspectRatio = 0.0f;
};
```

核心逻辑：

```cpp
void SceneCamera::RecalculateViewMatrix() {
    m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio,
                                     m_PerspectiveNear, m_PerspectiveFar);
}
```

| 要点 | 说明 |
|---|---|
| FOV 默认 45° | 使用 `glm::radians()` 转换，GLM 的 `perspective` 接受弧度 |
| AspectRatio 延迟计算 | 构造时为 0，首次 `SetViewportSize` 后生效 |
| 方法命名 `RecalculateViewMatrix` | ⚠️ 实际重算的是 Projection 矩阵，命名有误，应改为 `RecalculateProjection` |

#### 5. TransformComponent 与 CameraComponent

**新增：** `Components.h`

```cpp
struct TransformComponent {
    glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation    = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale       = { 1.0f, 1.0f, 1.0f };

    glm::mat4 GetTransform() const {
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        return glm::translate(glm::mat4(1.0f), Translation)
             * rotation
             * glm::scale(glm::mat4(1.0f), Scale);
    }
};

struct CameraComponent {
    SceneCamera Camera;
    bool Primary = true;
    bool FixedAspectRatio = false;
};
```

| 设计要点 | 说明 |
|---|---|
| TRS 分解存储 | 比直接存 mat4 更直观、更易序列化、更易编辑 |
| `glm::quat(Rotation)` | 从欧拉角构造四元数，避免万向锁 |
| 乘法顺序 T × R × S | 先缩放、再旋转、最后平移，符合标准 TRS 约定 |
| `CameraComponent` 预留 ECS | `Primary` 标记主相机，`FixedAspectRatio` 用于编辑器锁定比例 |
| 当前未接入 ECS | 作为纯数据结构使用，SandboxLayer 中手动构造实例 |

#### 6. SandboxLayer 端到端渲染验证

**修改：** `SandboxLayer.h` / `SandboxLayer.cpp`

完整渲染链路验证：

```cpp
// OnAttach: 资源创建 + 初始状态设置
m_Shader = Yue::Shader::Create("Simple", "Assets/Shaders/Simple.vert", "Assets/Shaders/Simple.frag");
m_VertexBuffer = Yue::VertexBuffer::Create(vertices, sizeof(vertices));
m_VertexBuffer->SetLayout({ {Yue::ShaderDataType::Float3, "a_Position"} });
m_VertexArray = Yue::VertexArray::Create();
m_VertexArray->AddVertexBuffer(m_VertexBuffer);
m_VertexArray->SetIndexBuffer(indexBuffer);

// Transform + Projection + View 一次性设置
Yue::TransformComponent transform;
transform.Translation = { 0.3f, 0.2f, 0.0f };
transform.Scale = { 1.0f, 1.0f, 1.0f };
m_Shader->SetMat4("transform", transform.GetTransform());

m_Camera.SetViewportSize(1280, 720);
m_Shader->SetMat4("projection", m_Camera.GetProjection());

glm::mat4 view = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 3.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
m_Shader->SetMat4("view", view);

// OnUpdate: 每帧绘制
Yue::Renderer::Submit(m_Shader, m_VertexArray);
```

**验证结果：** 三角形在透视投影下正确渲染，修改 View 矩阵可实现远近效果，修改 Transform 可控制物体位移。

### 🧠 今日核心知识点总结

| 知识点 | 要点 |
|---|---|
| `glUniformMatrix4fv` 列主序 | GLM 默认列主序存储，`transpose=GL_FALSE` 即可直接上传 |
| `glGetUniformLocation` 返回值 | -1 表示 uniform 不存在或被优化掉，不会报错但也不会生效 |
| Camera 单一职责 | Camera 只管 Projection；View 是"世界到相机"的变换，属于场景/实体层级 |
| TRS → Mat4 转换顺序 | T × R × S，四元数避免万向锁，`glm::quat(vec3)` 接受弧度欧拉角 |
| 编译防火墙延续 | OpenGLShader.h 仍使用前向声明，GL 依赖隔离在 .cpp 中 |

### 🏗️ 架构变化

```
之前（08-17）                          现在（08-19）
─────────────                          ─────────────
Shader (抽象)                          Shader (抽象)
 ├── Bind() / Unbind()                  ├── Bind() / Unbind()
 └── Create()                           ├── SetMat4() ← 新增
                                        └── Create()
OpenGLShader                           OpenGLShader
 ├── Compile / Link / Bind              ├── Compile / Link / Bind
 └── (无 Uniform 接口)                  └── SetMat4() ← 新增

(无 Camera 体系)                        Camera (基类) ← 新增
                                        ├── GetProjection()
                                        └── m_Projection (protected)
                                       SceneCamera ← 新增
                                        ├── SetPerspective()
                                        ├── SetViewportSize()
                                        └── RecalculateViewMatrix()

(无 Component 数据)                     Components.h ← 新增
                                        ├── TransformComponent (TRS + GetTransform)
                                        └── CameraComponent (Primary + FixedAR)

SandboxLayer                           SandboxLayer
 ├── VBO + VAO + Shader                ├── VBO + VAO + Shader
 └── Renderer::Submit                   ├── TransformComponent → SetMat4("transform")
                                        ├── SceneCamera → SetMat4("projection")
                                        ├── glm::lookAt → SetMat4("view")
                                        └── Renderer::Submit ← 完整 MVP 链路
```

**完整渲染数据流：**

```
TransformComponent.GetTransform() → SetMat4("transform")
SceneCamera.GetProjection()       → SetMat4("projection")
glm::lookAt(...)                  → SetMat4("view")
                                            ↓
                              Renderer::Submit(shader, vao)
                                            ↓
                              shader->Bind() → glUseProgram
                              vao->Bind()    → glBindVertexArray
                              DrawIndexed()  → glDrawElements
                                            ↓
                              Vertex Shader: gl_Position = P × V × T × pos
```

### ⚠️ 已知问题与待修复

| 问题 | 说明 | 优先级 |
|---|---|---|
| `RecalculateViewMatrix` 命名错误 | 实际重算的是 Projection 矩阵，应改为 `RecalculateProjection` | 🟡 中 |
| Uniform 仅在 OnAttach 设置一次 | 修改相机/物体参数后需重新 Bind Shader 并调用 SetMat4，否则不生效 | 🔴 高 |
| `SetMat4` 缺少编译/链接错误检查 | `glGetUniformLocation` 返回 -1 时无任何警告 | 🟡 中 |
| `SceneCamera` 构造时 AspectRatio=0 | `glm::perspective` 收到 0 宽高比会产生无效矩阵，需在首次 SetViewportSize 后才可用 | 🟡 中 |
| Shader 缺少 SetFloat / SetInt / SetVec3 等接口 | 当前仅支持 Mat4 | 📋 下阶段 |

### 📋 下一阶段计划

- [ ] 将 Uniform 设置移入 `OnUpdate`，支持运行时动态修改相机/物体参数
- [ ] 实现 Yaw/Pitch 相机控制，验证视角旋转效果
- [ ] 修复 `RecalculateViewMatrix` → `RecalculateProjection` 命名
- [ ] 添加 Shader 编译/链接错误日志（`glGetShaderInfoLog` / `glGetProgramInfoLog`）
- [ ] 实现更多 Uniform 接口（SetFloat, SetInt, SetVec3, SetVec4）
- [ ] 修复 OpenGLVertexArray 析构函数
- [ ] 考虑将 View 矩阵也纳入 SceneCamera 管理，提供完整的 VP 接口

---

## 2026-08-20