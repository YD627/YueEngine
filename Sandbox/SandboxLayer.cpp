#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(): Layer("SandboxLayer"){}

void SandboxLayer::OnAttach()
{
	YUE_INFO("Sandbox Attach");

	float vertices[] = {
		-0.5f, -0.8f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.3f,  0.5f, 0.0f
	};

	uint32_t indices[] = {
		0,1,2
	};

	m_Shader = Yue::Shader::Create("Simple", "Assets/Shaders/Simple.vert", "Assets/Shaders/Simple.frag");
	YUE_INFO("Shader Create");
	m_Shader->Bind();

	m_VertexBuffer = Yue::VertexBuffer::Create(vertices, sizeof(vertices));
	m_VertexBuffer->SetLayout({ {Yue::ShaderDataType::Float3,"a_Position"} });

	auto indexBuffer = Yue::IndexBuffer::Create(indices, 3);

	m_VertexArray = Yue::VertexArray::Create();
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);
	m_VertexArray->SetIndexBuffer(indexBuffer);
	
	// ---Object------
	m_Transform1.Translation = { -5.0f,-0.2f,0.0f };
	m_Transform1.Rotation = { 0.0f,0.0f,0.0f };
	m_Transform1.Scale = { 1.0f,1.0f,1.0f };

	m_Transform2.Translation = { 0.3f,0.2f,0.0f };
	m_Transform2.Rotation = { 0.0f,0.0f,3.0f };
	m_Transform2.Scale = { 1.0f,1.0f,1.0f };

	// ---Camera------
	m_Camera.SetViewportSize(1280, 720);

	Yue::TransformComponent cameraTransform;
	cameraTransform.Translation = { 0.0f,0.0f,10.0f };


	glm::mat4 view = glm::inverse(cameraTransform.GetTransform());
	Yue::Renderer::BeginScene(m_Camera, view);

}

void SandboxLayer::OnDetach()
{
	YUE_INFO("Sandbox Detach");
}

void SandboxLayer::OnUpdate(Yue::Timestep ts)
{
	Yue::Renderer::SetClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	Yue::Renderer::Clear();

	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform1.GetTransform());
	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform2.GetTransform());
}

void SandboxLayer::OnEvent(Yue::Event& event)
{
}