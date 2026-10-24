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
	Yue::Ref<Yue::Shader> m_Shader;
	Yue::Ref<Yue::Texture2D> m_TextureA;
	Yue::Ref<Yue::Texture2D> m_TextureB;
	Yue::Ref<Yue::Material> m_MaterialA;
	Yue::Ref<Yue::Material> m_MaterialB;
	Yue::Ref<Yue::Mesh> m_CubeMesh;

	// Test
	Yue::SceneCamera m_Camera;
	Yue::CameraController m_CameraController;
	Yue::TransformComponent m_Transform1;
	Yue::TransformComponent m_Transform2;
};