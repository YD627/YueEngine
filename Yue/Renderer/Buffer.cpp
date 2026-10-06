#include "Buffer.h"
#include "Renderer.h"
#include "Core/Assert.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Yue {
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: YUE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(size);
		}

		YUE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: YUE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		YUE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size) {
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: YUE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, size);
		}

		YUE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}