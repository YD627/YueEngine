#pragma once
#include "Renderer/Camera.h"

namespace Yue {
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetPerspective(float verticalFOV, float nearClip, float farClip);

	private:
		void RecalculateProjection();

	private:
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.1f, m_PerspectiveFar = 100.0f;

		float m_AspectRatio = 0.0f;
	};
}