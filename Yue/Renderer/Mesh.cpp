#include "Mesh.h"

#include "Core/Assert.h"
#include "Renderer/Buffer.h"

namespace Yue {
	Ref<Mesh> Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		// std::make_shared cannot call a private constructor, so construct here
		// where the static factory has access to it.
		return Ref<Mesh>(new Mesh(vertices, indices));
	}

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: m_VertexCount(static_cast<uint32_t>(vertices.size())),
		  m_IndexCount(static_cast<uint32_t>(indices.size()))
	{
		YUE_CORE_ASSERT(!vertices.empty(), "Mesh requires at least one vertex");
		YUE_CORE_ASSERT(!indices.empty(), "Mesh requires at least one index");

		const uint32_t vertexBufferSize = static_cast<uint32_t>(vertices.size() * sizeof(Vertex));

		// Create an empty VBO, then upload through the const-correct SetData API.
		auto vertexBuffer = VertexBuffer::Create(vertexBufferSize);
		vertexBuffer->SetData(vertices.data(), vertexBufferSize);
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Color" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});

		// glBufferData copies the indices immediately, so casting away const is safe.
		auto indexBuffer = IndexBuffer::Create(
			const_cast<uint32_t*>(indices.data()),
			static_cast<uint32_t>(indices.size()));

		m_VertexArray = VertexArray::Create();
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}
}
