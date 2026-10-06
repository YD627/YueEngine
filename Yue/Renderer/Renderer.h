#pragma once
#include "RendererAPI.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "Scene/SceneCamera.h"

namespace Yue {
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();

		static void BeginScene(SceneCamera& camera, glm::mat4& view);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData {
			glm::mat4 View;
			glm::mat4 Projection;
		};

		static Scope<SceneData> s_SceneData;
	};
}