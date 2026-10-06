#include "RendererAPI.h"
#include "Core/Assert.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Yue {
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case Yue::RendererAPI::API::None: YUE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
			case Yue::RendererAPI::API::OpenGL: return CreateScope<OpenGLRendererAPI>();
		}

		YUE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
