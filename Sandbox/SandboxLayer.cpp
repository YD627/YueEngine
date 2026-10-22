#include "SandboxLayer.h"

SandboxLayer::SandboxLayer()
	: Layer("SandboxLayer"),
	  m_CameraController(glm::vec3(0.0f, 0.0f, 10.0f))
{
}

void SandboxLayer::OnAttach()
{
	YUE_INFO("Sandbox Attach");

	float vertices[] = {
		// position              // color              // normal              // Texture Coordinates
		// Back (-Z)
		-0.5f, -0.5f, -0.5f,      1.0f, 0.0f, 0.0f,      0.0f,  0.0f, -1.0f,     1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,      0.0f, 1.0f, 0.0f,      0.0f,  0.0f, -1.0f,     0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,      0.0f, 0.0f, 1.0f,      0.0f,  0.0f, -1.0f,     0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 0.0f,      0.0f,  0.0f, -1.0f,     1.0f, 1.0f,

		// Front (+Z)
		-0.5f, -0.5f,  0.5f,      1.0f, 0.0f, 1.0f,      0.0f,  0.0f,  1.0f,     0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,      0.0f, 1.0f, 1.0f,      0.0f,  0.0f,  1.0f,     1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f,      0.0f,  0.0f,  1.0f,     1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 0.0f,      0.0f,  0.0f,  1.0f,     0.0f, 1.0f,

		// Left (-X)
		-0.5f, -0.5f, -0.5f,      1.0f, 0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,      1.0f, 0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,     1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,     0.0f, 1.0f,

		// Right (+X)
		 0.5f, -0.5f, -0.5f,      0.0f, 1.0f, 0.0f,      1.0f,  0.0f,  0.0f,     1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,      0.0f, 1.0f, 1.0f,      1.0f,  0.0f,  0.0f,     0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f,      1.0f,  0.0f,  0.0f,     0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,      0.0f, 0.0f, 1.0f,      1.0f,  0.0f,  0.0f,     1.0f, 1.0f,

		 // Top (+Y)
		 -0.5f,  0.5f, -0.5f,      1.0f, 1.0f, 0.0f,      0.0f,  1.0f,  0.0f,     0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,      0.0f, 0.0f, 1.0f,      0.0f,  1.0f,  0.0f,     1.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,      1.0f, 1.0f, 1.0f,      0.0f,  1.0f,  0.0f,     1.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 0.0f,      0.0f,  1.0f,  0.0f,     0.0f, 0.0f,

		 // Bottom (-Y)
		 -0.5f, -0.5f, -0.5f,      1.0f, 0.0f, 0.0f,      0.0f, -1.0f,  0.0f,     0.0f, 0.0f,
		  0.5f, -0.5f, -0.5f,      0.0f, 1.0f, 0.0f,      0.0f, -1.0f,  0.0f,     1.0f, 0.0f,
		  0.5f, -0.5f,  0.5f,      0.0f, 1.0f, 1.0f,      0.0f, -1.0f,  0.0f,     1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,      1.0f, 0.0f, 1.0f,      0.0f, -1.0f,  0.0f,     0.0f, 1.0f
	};

	uint32_t indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	m_Shader = Yue::Shader::Create("Simple", "Assets/Shaders/Simple.vert", "Assets/Shaders/Simple.frag");
	YUE_INFO("Shader Create");
	m_Shader->Bind();

	m_VertexBuffer = Yue::VertexBuffer::Create(vertices, sizeof(vertices));
	m_VertexBuffer->SetLayout({
		{ Yue::ShaderDataType::Float3, "a_Position" },
		{ Yue::ShaderDataType::Float3, "a_Color" },
		{ Yue::ShaderDataType::Float3, "a_Normal" },
		{ Yue::ShaderDataType::Float2, "a_TexCoord" }
	});
	m_Shader->SetFloat3("u_LightDirection", { -0.5f, -1.0f, -0.3f });

	auto indexBuffer = Yue::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

	m_VertexArray = Yue::VertexArray::Create();
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);
	m_VertexArray->SetIndexBuffer(indexBuffer);
	
	// ---Object------
	m_Transform1.Translation = { -4.0f, 0.0f, 0.0f };
	m_Transform1.Rotation = { 0.0f, 0.0f, 0.0f };
	m_Transform1.Scale = { 1.0f,1.0f,1.0f };

	m_Transform2.Translation = { 4.0f, 0.2f, -0.8f };
	m_Transform2.Rotation = { 0.0f, 0.0f, 0.0f };
	m_Transform2.Scale = { 1.0f,1.0f,1.0f };

	// ---Camera------
	m_Camera.SetViewportSize(1280, 720);
	YUE_INFO("Camera: hold RMB to look/move, WASD to move, Shift to speed up, Esc to release");

	// ---Texture------
	m_Texture = Yue::Texture2D::Create("Assets/Textures/cartoon_floor.png");
	if (!m_Texture || !m_Texture->IsLoaded())
	{
		YUE_ERROR("Failed to load texture: Assets/Textures/cartoon_floor.png");
	}
	else
	{
		YUE_INFO("Texture loaded");
		m_Shader->Bind();
		m_Shader->SetInt("u_Texture", 0);
	}
}

void SandboxLayer::OnDetach()
{
	Yue::Input::SetCursorMode(Yue::Input::CursorMode::Normal);
	YUE_INFO("Sandbox Detach");
}

void SandboxLayer::OnUpdate(Yue::Timestep ts)
{
	m_Transform1.Rotation.y += 1.0f * ts;
	m_Transform1.Rotation.x += 0.5f * ts;
	m_Transform2.Rotation.y -= 0.7f * ts;

	Yue::Renderer::SetClearColor(0.219f, 0.219f, 0.219f, 1.0f);
	Yue::Renderer::Clear();

	m_CameraController.OnUpdate(ts);

	glm::mat4 view = m_CameraController.GetViewMatrix();
	Yue::Renderer::BeginScene(m_Camera, view);

	if (m_Texture && m_Texture->IsLoaded())
	{
		m_Texture->Bind(0);
	}

	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform2.GetTransform());
	Yue::Renderer::Submit(m_Shader, m_VertexArray, m_Transform1.GetTransform());
	Yue::Renderer::EndScene();
}

void SandboxLayer::OnEvent(Yue::Event& event)
{
}
