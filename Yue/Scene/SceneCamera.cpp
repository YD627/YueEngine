#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Yue {
	SceneCamera::SceneCamera() {
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip) {
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height) {
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection() {
		m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
	}
}