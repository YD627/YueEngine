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

## 2026-10-11：Shader Uniform 更新机制完善

**日期：** 2026-10-11

### 一、今日目标

完善 YueEngine 中 Shader Uniform 的设置与更新机制，确保模型变换、相机视图矩阵和投影矩阵发生变化后，渲染结果能够正确更新。

### 二、今日完成内容

#### 1. 修正 Renderer::BeginScene 的调用时机

此前，`Renderer::BeginScene` 只在 `SandboxLayer::OnAttach` 中调用一次，导致视图矩阵和投影矩阵可能只是初始化时的状态。

现在改为在 `SandboxLayer::OnUpdate` 中每帧重新计算 View Matrix，并调用：

`Renderer::BeginScene(m_Camera, view)`

使相机变换能够及时反映到后续渲染中。

#### 2. 修正 SceneCamera::SetPerspective

`SceneCamera::SetPerspective()` 修改 FOV、Near Plane 或 Far Plane 后，现在会调用 `RecalculateProjection()`，重新计算投影矩阵。

这样可以避免相机投影参数已改变，但仍然使用旧投影矩阵的问题。

#### 3. 确认模型矩阵更新链路

`TransformComponent::GetTransform()` 在 `Renderer::Submit()` 调用前执行。

因此，物体的 Translation、Rotation 或 Scale 发生变化时，可以重新计算 Model Matrix，并通过现有渲染链路上传。

#### 4. 集中管理 Uniform 上传

当前 `Renderer::Submit()` 负责以下流程：

1. 绑定 Shader Program。
2. 上传 Model Matrix。
3. 上传 View Matrix。
4. 上传 Projection Matrix。
5. 绑定 VertexArray。
6. 调用 `RenderCommand::DrawIndexed()`。

这样可以让 Uniform 上传集中在 Renderer 的提交流程中，避免应用层直接管理底层 OpenGL 状态。

#### 5. 增加 Uniform Location 缓存

在 `OpenGLShader` 中新增 Uniform Location 缓存，避免每次调用 `SetMat4()` 都重新执行 `glGetUniformLocation()`。

缓存保留在 OpenGL 后端实现中，没有暴露到 `Shader` 抽象层，也没有放入 `Renderer`。

### 三、今日学习总结

- 理解了 View Matrix 和 Projection Matrix 不应只在初始化时计算一次，而应在相关参数变化时更新。
- 理解了 Model Matrix、View Matrix 和 Projection Matrix 在渲染流程中的职责。
- 理解了通过 `Renderer::Submit()` 集中管理 Shader 绑定、Uniform 上传和 Draw Call 的意义。
- 理解了 Uniform Location 缓存可以减少重复查询，并且适合放在 Shader 后端实现中管理。
- 进一步明确了应用层、Renderer 抽象层与 OpenGL 后端之间的职责划分。

### 四、当前限制

- 当前仍处于基础 3D 渲染阶段。
- 尚未完成完整的 3D Mesh、Texture 和 Material 系统。
- 需要继续验证实际运行时的相机移动、投影参数变化和物体变换效果。
- 暂未实现专门的 3D 相机控制器、复杂光照或阴影系统。

### 五、下一步计划

1. 完成 Day 2 的构建与运行验证，并确认矩阵变化确实能反映到画面中。
2. 实现一个能够正确显示深度遮挡的 3D Cube。
3. 检查并完善 OpenGL 深度测试状态。
4. 逐步实现可移动的 3D 相机。
5. 后续扩展 Mesh、Texture、Material 和基础光照。

### 六、验证记录

- [ ] 项目构建成功。
- [ ] 修改 Transform 后，物体变换能够反映到画面中。
- [ ] 修改相机视图参数后，画面能够正确更新。
- [ ] 修改 FOV、Near Plane 或 Far Plane 后，投影矩阵能够重新计算。
- [ ] 检查 Uniform Location 缓存的正确性。
- [ ] 检查 Git 差异并完成提交。

----

## 2026-10-12 ：基础 3D Cube 与深度测试
### 一、今日目标

在现有 YueEngine 架构的基础上，实现并验证基础 3D 立方体渲染，检查 Model、View、Projection 矩阵更新机制在三维场景中的实际效果，并确认深度测试能够正确处理物体之间的前后遮挡关系。

本次任务以验证现有渲染链路为主，没有扩展到纹理、材质或复杂光照系统。

### 二、今日完成内容

#### 1. 实现并运行 3D Cube

基于现有的 Shader、VertexArray、VertexBuffer、IndexBuffer 和 Renderer 等模块完成立方体绘制。

运行结果表明，YueEngine 已经能够正常显示 3D Cube，并通过旋转观察到立方体的不同面。

#### 2. 验证 Model / View / Projection 矩阵更新机制

在实际 3D 场景中验证 Day 2 完成的 Uniform 更新链路。

模型变换与相机相关矩阵能够参与当前帧的渲染，说明现有矩阵更新机制已在本次测试场景中正常工作。

#### 3. 验证深度测试

通过两个 Cube 的渲染测试，确认物体之间的遮挡关系正确。

测试结果表明：

- 两个 Cube 均能正常旋转。
- 不同深度下的表面能够正确进行遮挡。
- 改变物体的提交顺序后，未观察到前后遮挡关系错误。

这说明当前测试场景中的深度测试与深度缓冲清除流程能够正常工作。

#### 4. 验证渲染调用链

本次测试进一步验证了现有 Renderer、Shader、顶点数据和索引数据之间的协作关系。

目前，YueEngine 已经具备完成基础 3D 几何绘制所需的核心链路。

### 三、今日学习总结

- 加深了对三维几何、模型变换和透视观察的理解。
- 理解了深度测试在三维场景遮挡处理中的作用。
- 通过改变物体提交顺序，验证了深度测试不依赖简单的物体绘制先后顺序来决定最终可见表面。
- 将 2026-10-11 的 Uniform 更新机制应用到实际 3D 场景中，完成了从矩阵更新到三维绘制结果的验证。
- 进一步确认现有引擎基础架构可以继续扩展 3D 渲染能力，不需要因参考 Hazel 而重写整个项目。

### 四、当前能力与限制

**已验证的能力：**

- 基础 3D Cube 绘制。
- 立方体旋转与不同面的观察。
- Model / View / Projection 矩阵更新链路。
- 两个 Cube 之间的深度遮挡。
- 改变提交顺序后，遮挡关系仍然正确。

**尚未在本次任务中验证或实现的内容：**

- 完整的可交互 3D 相机控制器。
- 通用 Mesh 资源与模型加载系统。
- Texture 和 Material 系统。
- 基础光照、法线处理与阴影。
- 面向复杂场景的性能优化。

### 五、下一步计划

1. 检查现有相机与 Transform 的实现，决定是否需要完善 3D 相机控制。
2. 为顶点数据增加颜色属性，验证多属性 VertexBufferLayout。
3. 在此基础上学习顶点颜色插值或简单的面颜色效果。
4. 后续引入法线与 Blinn–Phong 光照，使立方体具备基础明暗效果。
5. 逐步完善 Mesh、Texture 和 Material 等 3D 渲染基础模块。

### 六、完成情况

Day 3 核心目标已通过实际运行验证：3D Cube 正常显示并旋转，两个 Cube 的深度遮挡正确，改变提交顺序后未观察到遮挡错误。

本次任务完成后，YueEngine 已具备经过实际运行验证的基础 3D 几何渲染能力。

----

## 2026-10-13：顶点属性与顶点颜色
### 一、今日目标

在 2026-10-12 已经实现的基础 3D 立方体上，为顶点添加颜色属性，理解顶点缓冲区的数据组织方式，并将顶点颜色传递给 Shader。

### 二、今日完成内容

#### 1. 为立方体顶点添加颜色

在原有立方体顶点数据的基础上，为每个顶点增加颜色数据，使顶点不仅包含位置，还包含颜色信息。

每个顶点目前包含两种属性：

- `a_Position`：顶点位置，使用 `Float3` 表示。
- `a_Color`：顶点颜色，使用 `Float3` 表示。

#### 2. 设置 VertexBuffer 的数据布局

通过 `SetLayout()` 描述顶点缓冲区中各项属性的组织方式：

```cpp
m_VertexBuffer->SetLayout({
    { Yue::ShaderDataType::Float3, "a_Position" },
    { Yue::ShaderDataType::Float3, "a_Color" }
});
```

通过布局描述，渲染系统能够根据属性类型和排列顺序确定顶点数据的读取方式，为后续增加法线、纹理坐标等顶点属性打下基础。

#### 3. 将顶点颜色传递给 Shader

在现有 Shader 流程中加入顶点颜色属性，使 Shader 能够读取顶点位置与颜色数据，并将颜色用于后续的片元颜色计算。

### 三、今日学习总结

1. 理解了顶点数据可以包含多个属性，而不只是位置。
2. 进一步认识了 `BufferLayout` 和 `SetLayout()` 的作用。
3. 理解了顶点缓冲区的数据组织方式需要与顶点属性配置相匹配。
4. 初步建立了从顶点数据、BufferLayout 到 Shader 输入的联系。

### 四、当前进度

YueEngine 已经具备基础 3D 立方体绘制能力，并在原有渲染流程上增加了顶点颜色数据。

当前学习重点由基础几何绘制进一步转向顶点属性组织，为后续实现顶点法线、基础光照和更完整的 Mesh 数据结构做准备。

### 五、后续计划

- 进一步理解 `BufferLayout` 中 stride 和 offset 的计算方式。
- 梳理 `OpenGLVertexArray` 如何将顶点布局转换为 OpenGL 顶点属性配置。
- 检查顶点颜色在三角形内部的插值效果。
- 在顶点属性流程稳定后，学习顶点法线与 Blinn–Phong 基础光照。

### 六、验证记录

本日已完成顶点颜色数据添加、VertexBuffer 布局设置和 Shader 数据传递。

运行效果、颜色插值效果以及 Day 3 深度测试等功能的回归情况，应以实际运行验证结果为准。

----

## 2026-10-18：基础漫反射光照

### 一、今日目标

为 YueEngine 中的 3D Cube 增加法线属性和基础方向光漫反射，实现从纯顶点颜色绘制到基础光照渲染的升级。
    
### 二、今日完成内容

1. 将 Cube 的顶点数据由 8 个共享顶点调整为 24 个逐面顶点，保留 36 个索引。
2. 为每个面设置独立的平面法线，支持硬边效果。
3. 新增 `a_Normal` 顶点属性，并映射到 Vertex Attribute Location 2。
4. 在 Vertex Shader 中使用法线矩阵变换法线。
5. 在 Fragment Shader 中根据光照方向和法线点积计算漫反射。
6. 为 `Shader` 和 `OpenGLShader` 新增 `SetFloat3`，用于上传 `u_LightDirection`。
7. 保留顶点颜色、MVP 变换、双 Cube 旋转、索引绘制和深度测试。

### 三、核心知识点

- 法线用于描述表面朝向，是基础光照计算的重要输入。
- 硬边模型需要允许相同几何位置具有不同的法线属性。
- 顶点数据、顶点布局和 Shader 输入必须保持一致。
- 法线变换与普通位置变换不同，非均匀缩放时需要使用法线矩阵。
- Lambert 漫反射通过法线与指向光源的方向之间的点积计算光照贡献，并将负值截断为零。

### 四、阶段成果

YueEngine 已实现基础方向光漫反射，能够在保留原有 3D 几何绘制和深度测试功能的基础上，根据 Cube 各个面的朝向呈现不同的明暗效果。

## 五、后续计划

Day 7 优先完善相机控制与观察变换，验证相机移动和旋转后，场景几何关系、深度测试与基础光照仍然正确。

----

## 2026-10-20：3D 自由相机控制器

### 一、今日目标

将固定视角改为可交互的 3D 自由相机，理解 View Matrix、yaw/pitch、鼠标输入与相机成像链路；
同时修复相机移动时物体“一帧帧”卡顿的问题，并验证 Day 6 的世界空间光照在相机运动后仍然正确。

### 二、今日完成内容

#### 1. 新增 CameraController

新增 `Yue/Scene/CameraController.h/.cpp`，负责相机的位姿与输入：

- 维护 `m_Position`、`m_Yaw`、`m_Pitch`；
- 按住鼠标右键捕获光标，鼠标移动控制视角；
- WASD 相对相机朝向移动，Space/LeftControl 升降，Shift 加速；
- pitch 限制在 ±89°，避免 forward 与 worldUp 共线导致 View Matrix 退化；
- 使用 `glm::lookAt(m_Position, m_Position + m_Front, m_Up)` 生成 View Matrix；
- 松开右键或按 Esc 释放光标；第一帧鼠标位置只用于建立参考点，避免视角跳变。

关键公式：

```cpp
front.x = cos(yaw) * cos(pitch);
front.y = sin(pitch);
front.z = sin(yaw) * cos(pitch);

right = normalize(cross(front, worldUp));
up    = normalize(cross(right, front));
```

#### 2. Input 增加光标模式

在 `Input` 中新增：

```cpp
enum class CursorMode { Normal, Hidden, Disabled };
static void SetCursorMode(CursorMode mode);
```

在 `GLFWInput.cpp` 中通过 `glfwSetInputMode(window, GLFW_CURSOR, ...)` 实现，
上层代码不直接依赖 GLFW。

#### 3. Sandbox 接入

- `SandboxLayer` 用 `CameraController m_CameraController` 替换原来的 `m_CameraTransform`；
- 原来：

```cpp
glm::mat4 view = glm::inverse(m_CameraTransform.GetTransform());
```

- 现在：

```cpp
m_CameraController.OnUpdate(ts);
glm::mat4 view = m_CameraController.GetViewMatrix();
```

- 初始相机位置仍为 `(0, 0, 10)`，初始视角与原来一致；
- `OnDetach` 恢复普通光标，避免退出后光标仍被锁定。

#### 4. 相机与投影的职责划分

本次没有把 FOV / aspect ratio 放进 `CameraController`，因为：

- `CameraController` 只负责相机位姿，生成 View Matrix；
- `SceneCamera` 负责镜头参数，生成 Projection Matrix；
- `Renderer::BeginScene(m_Camera, view)` 同时接收两者；
- 顶点着色器最终计算：

```glsl
gl_Position = projection * view * transform * vec4(aPos, 1.0);
```

当前投影参数来自 `SceneCamera`：

- 垂直 FOV：45°（`glm::radians(45.0f)`）；
- near：0.1，far：100；
- aspect ratio：`m_Camera.SetViewportSize(1280, 720)` 设置为 16:9。

#### 5. 修复移动卡顿

实际运行时发现相机移动时物体有一帧一帧的步进感。定位到主循环的时间计算：

```cpp
float time = std::chrono::duration<float, std::chrono::seconds::period>(
    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
float timeStep = time - m_LastFrameTime;
```

`high_resolution_clock` 的绝对时间（Windows 上通常是开机时长）被存进 `float`。
本机开机约 4.6 天，约 396600 秒，此时 `float` 的最小精度约为 31.25ms，
大于 60FPS 的 16.7ms 帧间隔，导致 `timeStep` 多数帧为 0，
物体只能每隔约两帧更新一次。

修复方式：

```cpp
auto lastFrameTime = std::chrono::steady_clock::now();

while (m_Running) {
    auto currentFrameTime = std::chrono::steady_clock::now();
    float timeStep = std::chrono::duration<float>(
        currentFrameTime - lastFrameTime).count();
    lastFrameTime = currentFrameTime;

    timeStep = std::min(timeStep, 0.1f);

    Timestep ts(timeStep);
    ...
}
```

同时在 `OpenGLContext::Init()` 中开启 VSync：

```cpp
glfwSwapInterval(1);
```

作用：

- 不再把绝对时间转成 `float`，而是用两个时间点的差；
- 避免暂停/断点后产生巨大的模拟步长；
- 稳定帧节奏，减少撕裂和抖动。

### 三、核心知识点

- View Matrix 是相机世界变换的逆；`lookAt(position, position + front, up)` 直接构造这个逆变换。
- `CameraController` 负责“相机在哪、朝哪看”，`SceneCamera` 负责“镜头多广、画面比例”。
- yaw 控制左右，pitch 控制上下；pitch 必须 clamp，避免 forward 与 worldUp 共线。
- 鼠标 delta 不乘 delta time；相机移动速度必须乘 delta time。
- 光照仍在世界空间计算；相机运动只改变 View，不改变 N 和 L，所以立方体各面的明暗关系不会“粘”在相机上。
- `float` 只有约 7 位有效数字；测量帧间隔应存储两个时间点并相减，而不是把绝对时间存进 `float`。
- 投影矩阵负责透视除法所需的信息；顶点最终还要经过透视除法、NDC、视口变换和光栅化才变成像素。

### 四、验证记录

- Debug|x64 编译通过，生成 `Yue.lib` 和 `Sandbox.exe`；仅有项目原有的 C4819 编码警告。
- 实际运行：窗口正常创建，Shader 编译/链接无错误，两个 Cube 正常渲染，深度测试正常。
- 模拟右键捕获/释放，日志确认 `Camera cursor captured` / `Camera cursor released`。
- 通过临时诊断日志确认：yaw 从 -90° 变为 -71.5°，pitch 从 0° 变为 -3.1°，位置从 `(0,0,10)` 前进到约 `(0.86, -0.15, 7.43)`，与当前 `front` 方向一致。
- 截图对比确认视角发生变化；相机移动后光照关系保持稳定。
- 修复时间精度后重新编译运行通过；实际移动手感待交互复验。
- 已知限制：
  - 窗口 resize 时相机 aspect ratio 与 viewport 尚未更新，画面可能被拉伸；
  - Back / Right / Top 三个面的三角绕序仍与法线方向不一致，当前未开启背面剔除，暂不影响画面；
  - 鼠标灵敏度受系统指针加速影响，实际手感可能需要调整；
  - `Time.cpp` 仍用 `float` 保存 `glfwGetTime()`，目前未被主循环调用，后续启用时需同样处理。

### 五、后续计划

- Day 8：纹理映射（UV、Texture2D、stb_image、纹理单元、过滤与 mipmap）。
- 处理窗口 resize 的 viewport 与投影宽高比。
- 在开启背面剔除前修正立方体 Back/Right/Top 的绕序。
- 考虑给相机增加滚轮 FOV 缩放，或把 `SceneCamera` 的引用接入控制器，用于统一调整镜头参数。

----

## 2026-10-22：纹理映射与 mipmap

### 一、今日目标

在 Day 6 的 Lambert 光照和 Day 7 的自由相机基础上，为 Cube 增加 UV 与 2D 纹理采样，
理解从图片文件到 `sampler2D` 的完整数据流，并加入 mipmap 解决远处缩小时的闪烁。
同时修复纹理对象生命周期导致的纯黑立方体问题。

### 二、今日完成内容

#### 1. 顶点增加 UV

每个顶点从 9 个 float 扩展为 11 个 float，UV 追加在 normal 后面，
这样前三个属性的 offset 完全不变：

| 属性 | 类型 | offset | 说明 |
|---|---|---|---|
| position | Float3 | 0 | 不变 |
| color | Float3 | 12 | 不变 |
| normal | Float3 | 24 | 不变 |
| texcoord | Float2 | 36 | 新增 |
| stride | | 44 | 9 → 11 个 float |

`BufferLayout` 增加：

```cpp
{ Yue::ShaderDataType::Float2, "a_TexCoord" }
```

Vertex Shader 增加：

```glsl
layout(location = 3) in vec2 aTexCoord;
out vec2 v_TexCoord;
```

每个面 4 个顶点使用 0–1 的 UV。由于当前是实体立方体、从外部观察六个面，
各面的 UV 方向按外部视角配置，不追求展开图意义上的统一。

#### 2. Texture2D 抽象与 OpenGL 实现

新增：

```text
Yue/Renderer/Texture.h
Yue/Renderer/Texture.cpp
Yue/Platform/OpenGL/OpenGLTexture.h
Yue/Platform/OpenGL/OpenGLTexture.cpp
```

`Texture` 提供宽度、高度、RendererID、`Bind(slot)` 和 `IsLoaded()`；
`Texture2D::Create(path)` 根据 `Renderer::GetAPI()` 分发到 `OpenGLTexture2D`，
与 `Shader::Create`、`VertexArray::Create` 的工厂模式保持一致。

`OpenGLTexture2D` 负责：

- 用 `stbi_load` 加载图片；
- 根据通道数选择 `GL_RGB8` / `GL_RGBA8`；
- `glCreateTextures` / `glTextureStorage2D` 创建不可变存储；
- `glTextureSubImage2D` 上传 level 0；
- 设置 `GL_REPEAT`、min/mag filter；
- `Bind(slot)` 使用 `glBindTextureUnit`。

#### 3. 使用 stb_image

本机已有：

```text
D:\OpenGL\Include\stb_image\stb_image.h
```

新增 `Yue/Platform/OpenGL/StbImage.cpp`：

```cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
```

通过一个独立的翻译单元定义实现，避免多个 `.cpp` 重复定义。
加载前调用 `stbi_set_flip_vertically_on_load(1)`，处理 stb_image 与 OpenGL 的 V 轴差异。

#### 4. Shader 采样与光照

Fragment Shader：

```glsl
vec3 texColor = texture(u_Texture, v_TexCoord).rgb;
vec3 tint = mix(vec3(1.0), v_Color, 0.5);

float diffuse = max(dot(normalize(v_Normal), normalize(-u_LightDirection)), 0.0);
float ambient = 0.15;

FragColor = vec4(texColor * tint * (diffuse + ambient), 1.0);
```

- 纹理作为基础反照率；
- 顶点颜色保留 50% 的染色影响；
- 加入 0.15 环境光，避免背光面纯黑。

`Shader` / `OpenGLShader` 新增 `SetInt`，在 `OnAttach` 中：

```cpp
m_Shader->Bind();
m_Shader->SetInt("u_Texture", 0);
```

绘制前：

```cpp
if (m_Texture && m_Texture->IsLoaded())
{
    m_Texture->Bind(0);
}
```

#### 5. 修复纯黑立方体

第一次运行时纹理加载成功，但立方体是纯黑。

原因：

```cpp
Yue::Ref<Yue::Texture2D> texture =
    Yue::Texture2D::Create("Assets/Textures/cartoon_floor.png");
```

`texture` 是 `OnAttach()` 的局部 `Ref`。函数返回时引用计数归零，
`OpenGLTexture2D` 析构并执行 `glDeleteTextures`，绘制时纹理单元 0 上已经没有有效纹理，
`sampler2D` 采样到默认/不完整纹理，结果为黑色。

修复：

- 把纹理提升为 `SandboxLayer` 的成员 `m_Texture`，让生命周期与 Layer 一致；
- 每帧绘制前显式 `Bind(0)`；
- 增加 `IsLoaded()` 检查；
- `OpenGLTexture2D` 的尺寸、ID、格式初始化为 0；
- 析构时只在 `m_RendererID != 0` 时删除；
- 上传前设置 `glPixelStorei(GL_UNPACK_ALIGNMENT, 1)`，避免 RGB 行对齐导致图像斜切；
- 修正 `SetInt` 中 uniform location 使用 `GLuint` 的问题，改为 `GLint`。

#### 6. mipmap

`OpenGLTexture2D` 现在分配完整的 mip 链：

```cpp
const int mipLevels = 1 + static_cast<int>(
    std::floor(std::log2(static_cast<float>(std::max(m_Width, m_Height)))));

glTextureStorage2D(m_RendererID, mipLevels, internalFormat, m_Width, m_Height);
```

上传 level 0 后：

```cpp
glGenerateTextureMipmap(m_RendererID);
```

过滤设置：

```cpp
glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

2048×2048 纹理对应 12 层 mip；三线性过滤在 mip 层内部做双线性，
在相邻 mip 层之间再做一次线性插值，减少远处缩小时的闪烁和摩尔纹。

### 三、核心知识点

- UV 是顶点的二维纹理坐标，光栅化时在三角形内部插值；
- 实体立方体每个面需要独立 UV，和逐面拆分法线是同一个思路；
- 纹理单元（`GL_TEXTURE0`）、`sampler2D` 与 `glUniform1i` 共同决定 Shader 从哪个单元采样；
- `Texture2D::Create` 工厂把平台相关实现隐藏在 `OpenGLTexture2D` 中；
- 纹理对象有 GPU 生命周期，局部 `Ref` 离开作用域会删除纹理，必须由需要它的对象持有；
- `glTextureStorage2D` 一次性分配不可变 mip 存储，`glGenerateTextureMipmap` 自动生成后续层；
- `GL_LINEAR_MIPMAP_LINEAR` 是层内双线性 + 层间线性（三线性过滤）；
- RGB 图像每行不保证 4 字节对齐，上传前应设置 `GL_UNPACK_ALIGNMENT`；
- 当前纹理按 `GL_RGBA8` / `GL_RGB8` 上传，尚未处理 sRGB 与线性空间转换。

### 四、验证记录

- Debug|x64 编译通过，生成 `Yue.lib` 和 `Sandbox.exe`；仅有原有的 C4819 编码警告。
- 运行日志确认纹理加载成功：

```text
[Yue Engine] Texture loaded: Assets/Textures/cartoon_floor.png
             (2048x2048, 12 mip levels)
```

- 修复纹理生命周期后，立方体不再纯黑，截图中可以看到卡通地面纹理。
- 当前 Shader 中顶点颜色仍会染色，纹理整体偏暗属于 `texColor * tint * (diffuse + ambient)` 的组合结果，可通过降低 tint 比例或提高 ambient 调整。
- 六个面的 UV 按外部观察方向配置，使用带方向性的贴图时可以逐面检查朝向。
- mipmap 的远距离闪烁改善需要实际移动相机观察，当前已验证纹理完整、12 层 mip 生成成功。

### 五、已知限制与后续计划

- 尚未处理 sRGB：纹理采样与光照都还没有做线性空间/伽马校正；
- 尚未开启各向异性过滤，斜视角下可能偏模糊；
- `Texture2D::Create(width, height)` 只有声明，没有实现，后续创建空纹理/FBO 时需要补上；
- 窗口 resize 时相机 aspect ratio 与 viewport 仍未更新；
- Back / Right / Top 三个面的绕序仍与法线方向不一致，开启背面剔除前需要修正；
- Day 9 可以考虑 Mesh 抽象与多物体/多材质，或先做 sRGB + Blinn-Phong；
- 之后再考虑 ImGui 调试面板、阴影映射和 PBR。

----

## 2026-10-23：Mesh 抽象与多物体/多纹理

### 一、今日目标

把 Cube 的顶点数据、索引和 VAO 从 `SandboxLayer` 抽到引擎层 `Mesh`，
让同一个 Mesh 可以被多个物体复用，并用不同纹理验证多物体渲染。

### 二、今日完成内容

#### 1. 新增 Vertex 结构

在 `Yue/Renderer/Mesh.h` 中定义：

```cpp
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};
```

内存布局：

| 属性 | 类型 | offset |
|---|---|---|
| Position | Float3 | 0 |
| Color | Float3 | 12 |
| Normal | Float3 | 24 |
| TexCoord | Float2 | 36 |
| stride | | 44 |

并加入：

```cpp
static_assert(sizeof(Vertex) == 44, "...");
```

保证 `Vertex` 的内存布局和 `BufferLayout` 的 stride 一致。

#### 2. 新增 Mesh 类

`Mesh` 负责：

- 接收 `std::vector<Vertex>` 和 `std::vector<uint32_t>`；
- 创建 VBO，上传顶点数据；
- 配置 `BufferLayout`；
- 创建 IndexBuffer；
- 创建 VAO，把 VBO/IBO 绑定进去；
- 对外只暴露 `GetVertexArray()`、顶点数、索引数。

`Mesh` 持有 `Ref<VertexArray>`，VAO 内部又持有 VBO/IBO 的 `Ref`，
所以 Mesh 的生命周期覆盖整条 GPU 资源链。

#### 3. Sandbox 接入 Mesh

`SandboxLayer` 原来手写：

```cpp
float vertices[] = { ... };
m_VertexBuffer = VertexBuffer::Create(...);
m_VertexArray = VertexArray::Create();
```

现在改成：

```cpp
std::vector<Yue::Vertex> vertices = { ... };
std::vector<uint32_t> indices = { ... };

m_CubeMesh = Yue::Mesh::Create(vertices, indices);
```

绘制时：

```cpp
Renderer::Submit(
    m_Shader,
    m_CubeMesh->GetVertexArray(),
    m_Transform2.GetTransform());
```

#### 4. 两个物体、两张纹理

新增第二张纹理：

```text
Sandbox/Assets/Textures/square_pavers_diff_1k.jpg
```

两个 Cube 在每次绘制前分别绑定自己的纹理：

```cpp
m_TextureA->Bind(0);
Renderer::Submit(m_Shader, m_CubeMesh->GetVertexArray(), m_Transform2.GetTransform());

m_TextureB->Bind(0);
Renderer::Submit(m_Shader, m_CubeMesh->GetVertexArray(), m_Transform1.GetTransform());
```

### 三、核心知识点

- `Vertex` 的结构体布局必须和 `BufferLayout` 的 offset/stride 一致；
- VAO 记录 VBO/IBO 的绑定和属性格式，Mesh 持有 VAO 即可管理整条资源链；
- 一个 Mesh 可以被多个物体复用，区别在 Transform 和 Material；
- `std::make_shared` 不能调用私有构造函数，因此 `Mesh::Create` 使用
  `Ref<Mesh>(new Mesh(...))`；
- `IndexBuffer::Create` 目前接收非 const 指针，Mesh 中使用 `const_cast`，
  因为 `glBufferData` 会立即复制索引数据。

### 四、验证记录

- Debug|x64 编译通过。
- 运行日志确认两张纹理加载成功：

```text
Texture loaded: Assets/Textures/cartoon_floor.png (2048x2048, 12 mip levels)
Texture loaded: Assets/Textures/square_pavers_diff_1k.jpg (1024x1024, 11 mip levels)
```

- 两个 Cube 使用同一个 Mesh、不同纹理，索引绘制、深度测试和自由相机正常。
- 画面整体偏暗来自 `texColor * tint * (diffuse + ambient)` 的组合，
  不是 Mesh 生命周期或布局问题。

### 五、已知限制与后续计划

- Mesh 的 VBO 目前通过 `Create(size)` + `SetData` 上传，内部使用
  `GL_DYNAMIC_DRAW`；静态网格后续可以增加 const-correct 的
  `Create(const void*, size)` 接口，使用 `GL_STATIC_DRAW`；
- 纹理仍然在 Sandbox 中手动绑定，下一步抽 Material；
- 新纹理 JPG 需要加入 Git；
- `Texture2D::Create(width, height)` 仍只有声明，没有实现；
- 窗口 resize、Back/Right/Top 绕序仍待处理。

----

## 2026-10-24：Material 封装

### 一、今日目标

把“Shader + 纹理 + sampler uniform”从 `SandboxLayer` 收进 `Material`，
让 Renderer 通过 Material 提交绘制，为后续多材质和模型加载做准备。

### 二、今日完成内容

#### 1. 新增 Material 类

`Material` 负责：

- 持有 `Ref<Shader>`；
- 保存若干纹理绑定：uniform 名字、`Ref<Texture2D>`、纹理单元；
- `Bind()` 时绑定 Shader、绑定纹理、设置 `sampler2D` uniform。

接口：

```cpp
class Material
{
public:
    static Ref<Material> Create(const Ref<Shader>& shader);

    void SetTexture(const std::string& uniformName,
        const Ref<Texture2D>& texture,
        uint32_t slot = 0);

    void Bind() const;

    const Ref<Shader>& GetShader() const { return m_Shader; }

private:
    struct TextureBinding
    {
        std::string UniformName;
        Ref<Texture2D> Texture;
        uint32_t Slot;
    };

    Ref<Shader> m_Shader;
    std::vector<TextureBinding> m_Textures;
};
```

`Material::Bind()`：

```cpp
m_Shader->Bind();

for (const auto& binding : m_Textures)
{
    if (binding.Texture && binding.Texture->IsLoaded())
    {
        binding.Texture->Bind(binding.Slot);
        m_Shader->SetInt(binding.UniformName,
            static_cast<int>(binding.Slot));
    }
}
```

必须先绑定 Shader，再设置 `sampler2D` uniform，否则 `SetInt` 会作用到错误的 Program。

#### 2. Renderer 增加 Material 重载

保留原有：

```cpp
Submit(const Ref<Shader>&, const Ref<VertexArray>&, const glm::mat4&);
```

新增：

```cpp
Submit(const Ref<Material>&, const Ref<VertexArray>&, const glm::mat4&);
```

实现中：

- `material->Bind()` 负责 Shader、纹理、采样器；
- Renderer 负责 `transform/view/projection`；
- 最后绑定 VAO 并 `DrawIndexed`。

职责划分：

| 内容 | 负责者 |
|---|---|
| 顶点/索引/VAO | Mesh |
| Shader + 纹理 + 材质 uniform | Material |
| 物体位置/旋转/缩放 | TransformComponent |
| View / Projection / Draw Call | Renderer |
| 方向光 | 场景/Renderer（当前仍在 Sandbox 设置） |

#### 3. Sandbox 接入 Material

```cpp
m_MaterialA = Yue::Material::Create(m_Shader);
m_MaterialA->SetTexture("u_Texture", m_TextureA, 0);

m_MaterialB = Yue::Material::Create(m_Shader);
m_MaterialB->SetTexture("u_Texture", m_TextureB, 0);
```

绘制：

```cpp
Renderer::Submit(m_MaterialA, m_CubeMesh->GetVertexArray(), m_Transform2.GetTransform());
Renderer::Submit(m_MaterialB, m_CubeMesh->GetVertexArray(), m_Transform1.GetTransform());
```

`OnUpdate` 中不再手动 `Bind` 纹理，也不再手动 `SetInt("u_Texture", 0)`。

#### 4. 代码检查与修正

检查中发现并修复：

- `SetTexture` 原来使用 `for (auto binding : m_Textures)`，会拷贝条目，
  重复设置同一个 uniform 时不会修改 vector；改为 `for (auto& binding : ...)`；
- `GetShader` 原来按值返回 `Ref<Shader>`，改为返回 const 引用，避免多余的
  `shared_ptr` 引用计数操作；
- 删除 `Material.h` 中 `#include "Core/Base.h";` 末尾多余的分号；
- 删除 Sandbox 中重复的 `m_Shader->Bind()` / `SetInt("u_Texture", 0)`；
- 删除 Sandbox 中注释掉的旧 `m_VertexBuffer` / `m_VertexArray` 成员。

### 三、核心知识点

- Material 是“表面外观”的数据封装，Mesh 是几何，Transform 是位置；
- 同一个 Shader 可以被多个 Material 共享，每个 Material 使用不同纹理；
- Material 持有 `Ref<Texture2D>` 和 `Ref<Shader>`，保证资源生命周期；
- 纹理单元 + uniform 名字共同决定 Shader 从哪个单元采样；
- `Material::Bind()` 必须先绑定 Shader，再 `SetInt`；
- 相机、View/Projection、灯光属于场景状态，不应该放进 Material；
- 当前 Material 只封装 Shader + 纹理，还没有材质参数和 MaterialInstance。

### 四、验证记录

- Debug|x64 编译通过，只有项目原有的 C4819 编码警告。
- 运行日志确认两张纹理加载成功：

```text
Texture loaded: Assets/Textures/cartoon_floor.png (2048x2048, 12 mip levels)
Texture loaded: Assets/Textures/square_pavers_diff_1k.jpg (1024x1024, 11 mip levels)
```

- 两个 Cube 分别使用 `m_MaterialA` / `m_MaterialB`，纹理显示正常，
  索引绘制、深度测试、自由相机和 Lambert 光照均正常。
- 没有出现 Material 生命周期导致的纹理丢失或黑屏。

### 五、已知限制与后续计划

- Material 目前只保存 Shader + 纹理绑定，还没有 `Tint`、`Roughness` 等材质参数；
- 如果多个物体共用 Material 但需要各自不同的参数，后续需要
  `MaterialInstance`；
- `Renderer::Submit(Shader)` 和 `Submit(Material)` 有重复的矩阵上传代码，
  后续可以抽一个内部 helper；
- 纹理单元仍然由调用者显式指定，后续可以根据 uniform 名字自动分配；
- 新纹理 JPG 还未加入 Git；
- 窗口 resize、Back/Right/Top 绕序、sRGB、各向异性过滤仍未处理；
- `Texture2D::Create(width, height)` 仍只有声明，没有实现；
- 下一步可以做 Material 参数/MaterialInstance，或者接 Assimp 模型加载；
- 之后再做多光源/Blinn-Phong、阴影映射和 ImGui。

----

## 2026-10-26：Assimp 模型加载与 EBO 状态修复

### 一、今日目标

接入 Assimp，把外部 OBJ 模型转换成引擎的 `Mesh`，并用 `Material` 绘制；
同时排查“加入模型后，原先两个 Cube 出现缺面”的回归问题。

### 二、今日完成内容

#### 1. Assimp 工程配置

本机已有：

```text
D:\OpenGL\Include\assimp
D:\OpenGL\Lib\assimp-vc145-mtd.lib
D:\OpenGL\Bin\x64\assimp-vc145-mtd.dll
```

在 `Sandbox.vcxproj` 的 Debug|x64 中：

- `AdditionalDependencies` 增加 `assimp-vc145-mtd.lib`；
- `AdditionalLibraryDirectories` 增加 `D:\OpenGL\Lib`；
- post-build 把 `assimp-vc145-mtd.dll` 复制到 `$(OutDir)`。

#### 2. 新增 Model 类

新增 `Yue/Renderer/Model.h/.cpp`：

- 构造时用 `Assimp::Importer` 读取模型；
- 使用 `aiProcess_Triangulate | aiProcess_FlipUVs`；
- 递归遍历 `aiNode`，把每个 `aiMesh` 转换成 `Mesh`；
- 保存 `std::vector<Ref<Mesh>>`；
- `Draw(material, transform)` 对每个子 Mesh 调用 `Renderer::Submit`。

顶点转换：

```text
aiMesh::mVertices        → Vertex::Position
aiMesh::mNormals         → Vertex::Normal
aiMesh::mTextureCoords[0] → Vertex::TexCoord
无顶点颜色               → Vertex::Color = 白色
aiFace::mIndices         → std::vector<uint32_t>
```

模型没有顶点颜色时使用白色，避免把纹理乘黑。

#### 3. 本地测试模型

当前网络不可用，无法下载现成模型，因此在本地生成了：

```text
Sandbox/Assets/Models/Hut.obj
```

模型包含：

- 4 面墙；
- 4 个屋顶三角形；
- 一个烟囱；
- 48 个顶点、22 个三角形；
- 逐面法线和 UV，保留硬边。

#### 4. Sandbox 接入

`SandboxLayer` 增加：

```cpp
Yue::Ref<Yue::Model> m_Model;
Yue::TransformComponent m_ModelTransform;
```

加载：

```cpp
m_Model = Yue::CreateRef<Yue::Model>("Assets/Models/Hut.obj");
m_ModelTransform.Scale = { 1.2f, 1.2f, 1.2f };
```

绘制：

```cpp
if (m_Model && m_Model->IsLoaded())
{
    m_Model->Draw(m_MaterialB, m_ModelTransform.GetTransform());
}
```

#### 5. `.gitignore` 放行 OBJ 资产

`.gitignore` 原本有：

```gitignore
*.obj
```

这会把模型文件 `Hut.obj` 当成编译器对象文件忽略掉。已追加：

```gitignore
!Sandbox/Assets/Models/
!Sandbox/Assets/Models/*.obj
```

#### 6. 修复 EBO 状态污染导致的 Cube 缺面

加入模型后，原先两个 Cube 出现缺面、三角形错位、能看到背景。  
用“只输出纹理”的临时 Shader 验证，问题仍然存在，说明不是光照或顶点颜色。

根因在 `OpenGLIndexBuffer` 构造函数：

```cpp
glCreateBuffers(1, &m_RendererID);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, ...);
```

`GL_ELEMENT_ARRAY_BUFFER` 的绑定是**当前 VAO 的状态**。

创建模型 Mesh 时：

- 当前绑定的 VAO 还是 Cube 的 VAO；
- 创建模型 IBO 时把模型的索引缓冲绑定到了 Cube VAO；
- Cube VAO 的 EBO 被覆盖；
- 绘制 Cube 时用 Cube 的顶点 + 模型的索引，导致缺面/错位。

修复方式：使用 DSA 上传，不在构造函数中绑定 EBO：

```cpp
glCreateBuffers(1, &m_RendererID);
glNamedBufferData(m_RendererID,
    count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
```

`Bind()` 仍然保留，因为 `VertexArray::SetIndexBuffer` 需要在绑定新 VAO 后
把 EBO 记录进该 VAO。

### 三、核心知识点

- Assimp 把文件解析成 `aiScene`，每个 `aiMesh` 包含顶点、法线、UV 和面索引；
- `aiProcess_Triangulate` 保证输出三角形，`aiProcess_FlipUVs` 处理 V 轴；
- 当前实现忽略 `aiNode::mTransformation`，单节点 OBJ 可用，层级模型后续要补；
- VAO 记录 EBO 绑定，`glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ...)` 会修改当前 VAO；
- 创建 GPU buffer 应尽量使用 DSA，避免修改当前绑定状态；
- 模型通常没有顶点颜色，转换时用白色，避免纹理被乘黑；
- `Model` 与 `Mesh`、`Material` 的分工：
  - Model 负责加载和组织子 Mesh；
  - Mesh 负责几何；
  - Material 负责 Shader + 纹理。

### 四、验证记录

- Debug|x64 编译通过，Assimp DLL 已复制到输出目录。
- 运行日志确认：

```text
Texture loaded: Assets/Textures/cartoon_floor.png (2048x2048, 12 mip levels)
Texture loaded: Assets/Textures/square_pavers_diff_1k.jpg (1024x1024, 11 mip levels)
Model loaded: Assets/Models/Hut.obj (1 meshes)
```

- 修复 EBO 状态后，两个 Cube 恢复为实体，模型小屋也正常显示；
- 截图确认 Cube 不再缺面，模型屋顶/墙体/烟囱可见；
- 模型整体偏暗是当前光照/纹理组合的结果，不是加载失败。

### 五、已知限制与后续计划

- 当前 Model 忽略节点层级变换，加载 glTF/FBX 的多节点模型可能位置错误；
- 所有子 Mesh 共用一个 Material，尚未读取 Assimp 的 `aiMaterial` 和贴图路径；
- Assimp 只配置了 Debug 版，Release 还没接；
- Assimp 的 include/lib/DLL 路径硬编码为 `D:\OpenGL`，换机器需要改工程配置；
- 生成的 `Hut.obj` 只是测试资产，后续可替换成真实模型；
- `Texture2D::Create(width, height)` 仍未实现；
- 窗口 resize、Back/Right/Top 绕序、sRGB、各向异性过滤仍未处理；
- 下一步建议：
  1. 累积 `aiNode::mTransformation`，支持层级模型；
  2. 按 `aiMesh::mMaterialIndex` 加载每个子 Mesh 的 Material/贴图；
  3. 接多光源/Blinn-Phong 或阴影映射；
  4. 用真实模型替换 Hut，做 Showcase。



