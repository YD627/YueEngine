#pragma once
#include <glm/glm.hpp>
#include "Core/KeyCodes.h"
#include "Core/MouseCodes.h"

namespace Yue {
	class Input {
	public:
		enum class CursorMode
		{
			Normal = 0,
			Hidden,
			Disabled
		};

		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);

		static glm::vec2 GetMousePosition();

		static float GetMouseX();
		static float GetMouseY();

		static void SetCursorMode(CursorMode mode);
	};
}