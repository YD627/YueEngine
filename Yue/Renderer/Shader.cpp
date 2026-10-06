#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Yue {
	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}
		return nullptr;
	}
}