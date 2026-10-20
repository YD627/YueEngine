#include "CameraController.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Input.h"
#include "Core/KeyCodes.h"
#include "Core/Log.h"
#include "Core/MouseCodes.h"

namespace Yue {
	CameraController::CameraController(const glm::vec3& position)
		: m_Position(position)
	{
		UpdateCameraVectors();
		UpdateView();
	}

	void CameraController::OnUpdate(Timestep ts)
	{
		HandleCursorCapture();

		if (!m_CursorCaptured)
		{
			return;
		}

		UpdateMouseLook();
		UpdateMovement(ts);
		UpdateView();
	}

	void CameraController::HandleCursorCapture()
	{
		const bool rightMouseDown = Input::IsMouseButtonPressed(Mouse::ButtonRight);

		if (!rightMouseDown)
		{
			m_SuppressCapture = false;
		}

		// Esc is a safety release. Ignore the still-held right mouse button until
		// it is released, otherwise the next frame would capture the cursor again.
		if (Input::IsKeyPressed(Key::Escape) && (m_CursorCaptured || rightMouseDown))
		{
			m_SuppressCapture = true;
			if (m_CursorCaptured)
			{
				ReleaseCursor();
			}
		}

		if (rightMouseDown && !m_CursorCaptured && !m_SuppressCapture)
		{
			CaptureCursor();
		}
		else if (!rightMouseDown && m_CursorCaptured)
		{
			ReleaseCursor();
		}
	}

	void CameraController::UpdateMouseLook()
	{
		const glm::vec2 currentMouse = Input::GetMousePosition();

		// The first sample after capturing the cursor is only used to establish
		// the reference point; otherwise the camera would jump by the distance
		// from the old cursor position.
		if (m_FirstMouse)
		{
			m_LastMousePosition = currentMouse;
			m_FirstMouse = false;
			return;
		}

		const glm::vec2 delta = currentMouse - m_LastMousePosition;
		m_LastMousePosition = currentMouse;

		m_Yaw += delta.x * m_MouseSensitivity;
		m_Pitch -= delta.y * m_MouseSensitivity;

		// Clamp before the forward vector is rebuilt. At exactly +/-90 degrees
		// the forward vector becomes parallel to world up and the view matrix
		// would degenerate.
		m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

		UpdateCameraVectors();
	}

	void CameraController::UpdateMovement(Timestep ts)
	{
		glm::vec3 direction(0.0f);

		if (Input::IsKeyPressed(Key::W)) direction += m_Front;
		if (Input::IsKeyPressed(Key::S)) direction -= m_Front;
		if (Input::IsKeyPressed(Key::D)) direction += m_Right;
		if (Input::IsKeyPressed(Key::A)) direction -= m_Right;
		if (Input::IsKeyPressed(Key::Space)) direction += m_WorldUp;
		if (Input::IsKeyPressed(Key::LeftControl)) direction -= m_WorldUp;

		if (glm::length(direction) > 0.0001f)
		{
			// Normalize so diagonal movement is not faster than axis-aligned movement.
			direction = glm::normalize(direction);

			const float speed = m_MoveSpeed
				* (Input::IsKeyPressed(Key::LeftShift) ? m_SpeedMultiplier : 1.0f);

			m_Position += direction * speed * ts.GetSeconds();
		}
	}

	void CameraController::UpdateCameraVectors()
	{
		const float yaw = glm::radians(m_Yaw);
		const float pitch = glm::radians(m_Pitch);

		// With yaw = -90 degrees and pitch = 0, front points down -Z, which is
		// the same initial viewing direction as the old camera transform.
		glm::vec3 front;
		front.x = std::cos(yaw) * std::cos(pitch);
		front.y = std::sin(pitch);
		front.z = std::sin(yaw) * std::cos(pitch);

		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}

	void CameraController::UpdateView()
	{
		m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void CameraController::CaptureCursor()
	{
		Input::SetCursorMode(Input::CursorMode::Disabled);
		m_CursorCaptured = true;
		m_FirstMouse = true;
		YUE_CORE_INFO("Camera cursor captured (hold RMB to look/move, Esc to release)");
	}

	void CameraController::ReleaseCursor()
	{
		Input::SetCursorMode(Input::CursorMode::Normal);
		m_CursorCaptured = false;
		YUE_CORE_INFO("Camera cursor released");
	}
}
