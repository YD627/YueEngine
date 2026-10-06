#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include "Core/Base.h"

namespace Yue {
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;

		//static Ref<Shader> Create(const std::string& filepath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}