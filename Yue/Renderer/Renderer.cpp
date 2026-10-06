#include "Renderer.h"

namespace Yue {
	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init() {
		RenderCommand::Init();
	}

	void Renderer::Shutdown() {
		// TODO
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::SetClearColor(float r, float g, float b, float a) {
		RenderCommand::SetClearColor(r, g, b, a);
	}

	void Renderer::Clear() {
		RenderCommand::Clear();
	}

	void Renderer::BeginScene(SceneCamera& camera, glm::mat4& view) {
		s_SceneData->View = view;
		s_SceneData->Projection = camera.GetProjection();
	}

	void Renderer::EndScene() {
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform) {
		shader->Bind();
		shader->SetMat4("transform", transform);
		shader->SetMat4("view", s_SceneData->View);
		shader->SetMat4("projection", s_SceneData->Projection);

		vertexArray->Bind();

		RenderCommand::DrawIndexed(vertexArray);
	}
}