#include "Material.h"

#include "Core/Assert.h"

namespace Yue {
	Ref<Material> Material::Create(const Ref<Shader>& shader) {
		return Ref<Material>(new Material(shader));
	}

	Material::Material(const Ref<Shader>& shader):m_Shader(shader) {
		YUE_CORE_ASSERT(m_Shader, "Material requires a shader");
	}

	void Material::SetTexture(const std::string& uniformName,
		const Ref<Texture2D>& texture,
		uint32_t slot) {

		for (auto& binding : m_Textures) {
			if (binding.UniformName == uniformName) {
				binding.Texture = texture;
				binding.Slot = slot;
				return;
			}
		}
		m_Textures.push_back({ uniformName,texture,slot });
	}

	void Material::Bind() const {
		m_Shader->Bind();

		for (const auto& binding : m_Textures) {
			if (binding.Texture && binding.Texture->IsLoaded()) {
				binding.Texture->Bind(binding.Slot);
				m_Shader->SetInt(binding.UniformName, static_cast<int>(binding.Slot));
			}
		}
	}
}