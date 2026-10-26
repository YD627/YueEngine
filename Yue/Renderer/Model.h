#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Core/Base.h"
#include "Renderer/Mesh.h"

namespace Yue {
	class Material;

	class Model
	{
	public:
		explicit Model(const std::string& path);

		void Draw(const Ref<Material>& material, const glm::mat4& transform) const;

		const std::vector<Ref<Mesh>>& GetMeshes() const { return m_Meshes; }
		bool IsLoaded() const { return m_IsLoaded; }

	private:
		std::vector<Ref<Mesh>> m_Meshes;
		bool m_IsLoaded = false;
	};
}
