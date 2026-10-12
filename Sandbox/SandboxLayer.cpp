#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(): Layer("SandboxLayer"){}

void SandboxLayer::OnAttach()
{
	YUE_INFO("Sandbox Attach");

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f
	};

	uint32_t indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		0, 4, 7, 7, 3, 0,
		1, 5, 6, 6, 2, 1,
		3, 2, 6, 6, 7, 3,
		0, 1, 5, 5, 4, 0
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
	m_Transform1.Translation = { 0.0f, 0.0f, 0.0f };
	m_Transform1.Rotation = { 0.0f, 0.0f, 0.0f };
	m_Transform1.Scale = { 1.0f,1.0f,1.0f };

	m_Transform2.Translation = { 0.3f, 0.2f, -0.8f };
	m_Transform2.Rotation = { 0.0f, 0.0f, 0.0f };
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
	m_Transform1.Rotation.y += 1.0f * ts;
	m_Transform1.Rotation.x += 0.5f * ts;
	m_Transform2.Rotation.y -= 0.7f * ts;

	Yue::Renderer::SetClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	Yue::Renderer::Clear();

	glm::mat4 view = glm::inverse(m_CameraTransform.GetTransform());
	Yue::Renderer::BeginScene(m_Camera, view);

	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform2.GetTransform());
	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform1.GetTransform());
	Yue::Renderer::EndScene();
}

void SandboxLayer::OnEvent(Yue::Event& event)
{
}
