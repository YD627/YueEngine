#pragma once

#include <glm/glm.hpp>

#include "Core/Timestep.h"

namespace Yue {
	class CameraController
	{
	public:
		explicit CameraController(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 10.0f));

		void OnUpdate(Timestep ts);

		const glm::mat4& GetViewMatrix() const { return m_View; }
		const glm::vec3& GetPosition() const { return m_Position; }
		bool IsCursorCaptured() const { return m_CursorCaptured; }

	private:
		void HandleCursorCapture();
		void UpdateMouseLook();
		void UpdateMovement(Timestep ts);
		void UpdateCameraVectors();
		void UpdateView();

		void CaptureCursor();
		void ReleaseCursor();

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		float m_Yaw = -90.0f;
		float m_Pitch = 0.0f;
		float m_MoveSpeed = 3.0f;
		float m_SpeedMultiplier = 2.5f;
		float m_MouseSensitivity = 0.1f;

		glm::vec2 m_LastMousePosition = glm::vec2(0.0f);
		bool m_FirstMouse = true;
		bool m_CursorCaptured = false;
		bool m_SuppressCapture = false;

		glm::mat4 m_View = glm::mat4(1.0f);
	};
}
