#include "VertexArray.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Yue {
	Ref<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexArray>();
		}
		return nullptr;
	}

}