#pragma once
#include "Renderer/Shader.h"

typedef unsigned int GLenum;
typedef int GLint;

namespace Yue {
	class OpenGLShader : public Shader
	{
	public:
		//OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;

	private:
		GLenum CompileShader(GLenum type, const std::string& source);
		GLenum CreateProgram(GLenum vertexShader, GLenum fragmentShader);
		GLint GetUniformLocation(const std::string& name);

	private:
		uint32_t m_RendererID;
		//std::string m_FilePath;
		std::string m_Name;
		std::unordered_map<std::string, GLint> m_UniformLocationCache;
	};
}
