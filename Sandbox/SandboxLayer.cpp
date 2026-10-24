#include "SandboxLayer.h"

SandboxLayer::SandboxLayer()
	: Layer("SandboxLayer"),
	  m_CameraController(glm::vec3(0.0f, 0.0f, 10.0f))
{
}

void SandboxLayer::OnAttach()
{
	YUE_INFO("Sandbox Attach");

	std::vector<Yue::Vertex> vertices = {
		// position                  // color                 // normal                // Texture Coordinates
		// Back (-Z)
		{{-0.5f, -0.5f, -0.5f},      {1.0f, 0.0f, 0.0f},      {0.0f,  0.0f, -1.0f},    {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},      {0.0f, 1.0f, 0.0f},      {0.0f,  0.0f, -1.0f},    {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},      {0.0f, 0.0f, 1.0f},      {0.0f,  0.0f, -1.0f},    {0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},      {1.0f, 1.0f, 0.0f},      {0.0f,  0.0f, -1.0f},    {1.0f, 1.0f}},

		// Front (+Z)
		{{-0.5f, -0.5f,  0.5f},      {1.0f, 0.0f, 1.0f},      {0.0f,  0.0f,  1.0f},    {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},      {0.0f, 1.0f, 1.0f},      {0.0f,  0.0f,  1.0f},    {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},      {1.0f, 1.0f, 1.0f},      {0.0f,  0.0f,  1.0f},    {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},      {0.0f, 0.0f, 0.0f},      {0.0f,  0.0f,  1.0f},    {0.0f, 1.0f}},

		// Left (-X)
		{{-0.5f, -0.5f, -0.5f},      {1.0f, 0.0f, 0.0f},      {-1.0f, 0.0f,  0.0f},    {0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},      {1.0f, 0.0f, 1.0f},      {-1.0f, 0.0f,  0.0f},    {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},      {0.0f, 0.0f, 0.0f},      {-1.0f, 0.0f,  0.0f},    {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},      {1.0f, 1.0f, 0.0f},      {-1.0f, 0.0f,  0.0f},    {0.0f, 1.0f}},

		// Right (+X)
		{{ 0.5f, -0.5f, -0.5f},      {0.0f, 1.0f, 0.0f},      {1.0f,  0.0f,  0.0f},    {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},      {0.0f, 1.0f, 1.0f},      {1.0f,  0.0f,  0.0f},    {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},      {1.0f, 1.0f, 1.0f},      {1.0f,  0.0f,  0.0f},    {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f},      {0.0f, 0.0f, 1.0f},      {1.0f,  0.0f,  0.0f},    {1.0f, 1.0f}},

		// Top (+Y)
		{{-0.5f,  0.5f, -0.5f},      {1.0f, 1.0f, 0.0f},      {0.0f,  1.0f,  0.0f},    {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f},      {0.0f, 0.0f, 1.0f},      {0.0f,  1.0f,  0.0f},    {1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},      {1.0f, 1.0f, 1.0f},      {0.0f,  1.0f,  0.0f},    {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},      {0.0f, 0.0f, 0.0f},      {0.0f,  1.0f,  0.0f},    {0.0f, 0.0f}},

		// Bottom (-Y)
		{{-0.5f, -0.5f, -0.5f},      {1.0f, 0.0f, 0.0f},      {0.0f, -1.0f,  0.0f},    {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},      {0.0f, 1.0f, 0.0f},      {0.0f, -1.0f,  0.0f},    {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},      {0.0f, 1.0f, 1.0f},      {0.0f, -1.0f,  0.0f},    {1.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},      {1.0f, 0.0f, 1.0f},      {0.0f, -1.0f,  0.0f},    {0.0f, 1.0f}}
	};

	std::vector<uint32_t> indices = {
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

	m_CubeMesh = Yue::Mesh::Create(vertices, indices);

	m_Shader->SetFloat3("u_LightDirection", { -0.5f, -1.0f, -0.3f });
	
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
	m_TextureA = Yue::Texture2D::Create("Assets/Textures/cartoon_floor.png");
	if (!m_TextureA || !m_TextureA->IsLoaded())
	{
		YUE_ERROR("Failed to load texture: Assets/Textures/cartoon_floor.png");
	}
	else
	{
		YUE_INFO("Texture loaded");
		
	}

	m_TextureB = Yue::Texture2D::Create("Assets/Textures/square_pavers_diff_1k.jpg");
	if (!m_TextureB || !m_TextureB->IsLoaded())
	{
		YUE_ERROR("Failed to load texture: Assets/Textures/square_pavers_diff_1k.jpg");
	}
	else
	{
		YUE_INFO("Texture loaded");
	}

	// ---Material------
	m_MaterialA = Yue::Material::Create(m_Shader);
	m_MaterialA->SetTexture("u_Texture", m_TextureA, 0);

	m_MaterialB = Yue::Material::Create(m_Shader);
	m_MaterialB->SetTexture("u_Texture", m_TextureB, 0);
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

	Yue::Renderer::Submit(m_MaterialA, m_CubeMesh->GetVertexArray(), m_Transform2.GetTransform());
	Yue::Renderer::Submit(m_MaterialB, m_CubeMesh->GetVertexArray(), m_Transform1.GetTransform());

	Yue::Renderer::EndScene();
}

void SandboxLayer::OnEvent(Yue::Event& event)
{
}
