#include "GLFWWindow.h"
#include "Event/ApplicationEvent.h"

namespace Yue {
	GLFWWindow::GLFWWindow(const WindowProps& props)
	{
		Init(props);

		m_context = GraphicsContext::Create(m_Window);
		m_context->Init();
	}

	void GLFWWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		glfwInit();
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initialize GLFW");
		}
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		if (!m_Window)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}
		glfwMakeContextCurrent(m_Window);

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWWindow::Update()
	{	
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	bool GLFWWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	unsigned int GLFWWindow::GetWidth() const
	{
		return m_Data.Width;
	}

	unsigned int GLFWWindow::GetHeight() const
	{
		return m_Data.Height;
	}
}