#include "Window.h"
#include "Platform/GLFW/GLFWWindow.h"

namespace Yue {
	Scope<Window> Window::Create(const WindowProps& props) {
		return CreateScope<GLFWWindow>(props);
	}
}