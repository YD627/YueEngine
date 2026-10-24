#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include "Core/Base.h"
#include "Renderer/VertexArray.h"

namespace Yue {
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	// BufferLayout computes offsets sequentially, so the struct must stay tightly
	// packed: 3 + 3 + 3 + 2 floats = 44 bytes, matching the layout stride.
	static_assert(sizeof(Vertex) == 44, "Vertex size must match BufferLayout stride (44 bytes)");

	class Mesh
	{
	public:
		static Ref<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		uint32_t GetVertexCount() const { return m_VertexCount; }
		uint32_t GetIndexCount() const { return m_IndexCount; }

	private:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

		Ref<VertexArray> m_VertexArray;
		uint32_t m_VertexCount = 0;
		uint32_t m_IndexCount = 0;
	};
}
