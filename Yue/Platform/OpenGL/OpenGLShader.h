#pragma once
#include "Renderer/Shader.h"

typedef unsigned int GLenum;

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

	private:
		GLenum CompileShader(GLenum type, const std::string& source);
		GLenum CreateProgram(GLenum vertexShader, GLenum fragmentShader);

	private:
		uint32_t m_RendererID;
		//std::string m_FilePath;
		std::string m_Name;
	};
}