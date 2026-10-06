#include "Core/Input.h"
#include "GLFW/glfw3.h"

namespace Yue {
	bool Input::IsKeyPressed(KeyCode key) {
		auto window = static_cast<GLFWwindow*>(glfwGetCurrentContext());
		auto state = glfwGetKey(window, key);
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(MouseCode button) {
		auto window = static_cast<GLFWwindow*>(glfwGetCurrentContext());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition() {
		auto window = static_cast<GLFWwindow*>(glfwGetCurrentContext());
	    double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { (float)x, (float)y };
	}

	float Input::GetMouseX() {
		return GetMousePosition().x;
	}

	float Input::GetMouseY() {
		return GetMousePosition().y;
	}
}