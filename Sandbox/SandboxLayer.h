#pragma once
#include "Yue.h"

class SandboxLayer : public Yue::Layer
{
public:
	SandboxLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Yue::Timestep ts) override;
	void OnEvent(Yue::Event& event) override;

private:
	Yue::Ref<Yue::VertexBuffer> m_VertexBuffer;
	Yue::Ref<Yue::VertexArray> m_VertexArray;
	Yue::Ref<Yue::Shader> m_Shader;

	// Test
	Yue::SceneCamera m_Camera;
	Yue::TransformComponent m_Transform1;
	Yue::TransformComponent m_Transform2;
};