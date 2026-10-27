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

	void Material::SetFloat(const std::string& uniformName, float value) {
		for (auto& property : m_Floats) {
			if (property.UniformName == uniformName) {
				property.Value = value;
				return;
			}
		}
		m_Floats.push_back({ uniformName, value });
	}

	void Material::SetFloat3(const std::string& uniformName, const glm::vec3& value) {
		for (auto& property : m_Vec3s) {
			if (property.UniformName == uniformName) {
				property.Value = value;
				return;
			}
		}
		m_Vec3s.push_back({ uniformName, value });
	}

	void Material::Bind() const {
		m_Shader->Bind();

		for (const auto& binding : m_Textures) {
			if (binding.Texture && binding.Texture->IsLoaded()) {
				binding.Texture->Bind(binding.Slot);
				m_Shader->SetInt(binding.UniformName, static_cast<int>(binding.Slot));
			}
		}

		for (const auto& property : m_Floats) {
			m_Shader->SetFloat(property.UniformName, property.Value);
		}

		for (const auto& property : m_Vec3s) {
			m_Shader->SetFloat3(property.UniformName, property.Value);
		}
	}
}