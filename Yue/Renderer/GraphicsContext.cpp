#include "GraphicsContext.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Yue {
	Scope<GraphicsContext> GraphicsContext::Create(void* window) {
		return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
	}
}