#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Core/Base.h"
#include "Shader.h"
#include "Texture.h"

namespace Yue {
	class Material {
	public:
		static Ref<Material> Create(const Ref<Shader>& shader);

		void SetTexture(const std::string& uniformName,
			const Ref<Texture2D>& texture,
			uint32_t slot = 0);

		void SetFloat(const std::string& uniformName, float value);
		void SetFloat3(const std::string& uniformName, const glm::vec3& value);

		void Bind() const;

		const Ref<Shader>& GetShader() const { return m_Shader; }

	private:
		explicit Material(const Ref<Shader>& shader);

		struct TextureBinding {
			std::string UniformName;
			Ref<Texture2D> Texture;
			uint32_t Slot;
		};

		struct FloatProperty {
			std::string UniformName;
			float Value;
		};

		struct Vec3Property {
			std::string UniformName;
			glm::vec3 Value;
		};

		Ref<Shader> m_Shader;
		std::vector<TextureBinding> m_Textures;
		std::vector<FloatProperty> m_Floats;
		std::vector<Vec3Property> m_Vec3s;
	};
}