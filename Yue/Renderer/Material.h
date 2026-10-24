#pragma once
#include <string>
#include <vector>

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

		void Bind() const;

		const Ref<Shader>& GetShader() const { return m_Shader; }

	private:
		explicit Material(const Ref<Shader>& shader);

		struct TextureBinding {
			std::string UniformName;
			Ref<Texture2D> Texture;
			uint32_t Slot;
		};

		Ref<Shader> m_Shader;
		std::vector<TextureBinding> m_Textures;
	};
}