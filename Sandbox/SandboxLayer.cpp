#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(): Layer("SandboxLayer"){}

void SandboxLayer::OnAttach()
{
	YUE_INFO("Sandbox Attach");

	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};

	uint32_t indices[] = {
		0,1,2,
		1,3,2,

		4,5,7,
		4,6,7,

		3,1,5,
		3,5,7,

		2,4,1,
		2,6,4,

		2,3,6,
		3,7,6,

		0,4,5,
		0,5,1
	};

	m_Shader = Yue::Shader::Create("Simple", "Assets/Shaders/Simple.vert", "Assets/Shaders/Simple.frag");
	YUE_INFO("Shader Create");
	m_Shader->Bind();

	m_VertexBuffer = Yue::VertexBuffer::Create(vertices, sizeof(vertices));
	m_VertexBuffer->SetLayout({ {Yue::ShaderDataType::Float3,"a_Position"} });

	auto indexBuffer = Yue::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

	m_VertexArray = Yue::VertexArray::Create();
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);
	m_VertexArray->SetIndexBuffer(indexBuffer);
	
	// ---Object------
	m_Transform1.Translation = { -5.0f,-0.2f,0.0f };
	m_Transform1.Rotation = { 1.0f,2.0f,0.0f };
	m_Transform1.Scale = { 1.0f,1.0f,1.0f };

	m_Transform2.Translation = { 0.3f,0.2f,0.0f };
	m_Transform2.Rotation = { 1.0f,0.0f,3.0f };
	m_Transform2.Scale = { 1.0f,1.0f,1.0f };

	// ---Camera------
	m_Camera.SetViewportSize(1280, 720);

	m_CameraTransform.Translation = { 0.0f,0.0f,10.0f };

}

void SandboxLayer::OnDetach()
{
	YUE_INFO("Sandbox Detach");
}

void SandboxLayer::OnUpdate(Yue::Timestep ts)
{
	glm::mat4 view = glm::inverse(m_CameraTransform.GetTransform());
	Yue::Renderer::BeginScene(m_Camera, view);

	Yue::Renderer::SetClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	Yue::Renderer::Clear();

	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform1.GetTransform());
	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform2.GetTransform());
}

void SandboxLayer::OnEvent(Yue::Event& event)
{
}