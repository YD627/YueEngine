#include "OpenGLContext.h"
#include "Core/Assert.h"

#include "OpenGL.h"

namespace Yue {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle) {
		YUE_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init() {
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		YUE_CORE_ASSERT(status, "Failed to initialize Glad!");
		YUE_CORE_INFO("OpenGL Context Initialized");
	}

	void OpenGLContext::SwapBuffers() {
		glfwSwapBuffers(m_WindowHandle);
	}
}