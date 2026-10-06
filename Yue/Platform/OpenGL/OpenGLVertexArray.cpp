#include "OpenGLVertexArray.h"
#include "OpenGL.h"

namespace Yue {
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Yue::ShaderDataType::Float:	return GL_FLOAT;
			case Yue::ShaderDataType::Float2:	return GL_FLOAT;
			case Yue::ShaderDataType::Float3:	return GL_FLOAT;
			case Yue::ShaderDataType::Float4:	return GL_FLOAT;
			case Yue::ShaderDataType::Mat3:		return GL_FLOAT;
			case Yue::ShaderDataType::Mat4:		return GL_FLOAT;
			case Yue::ShaderDataType::Int:		return GL_INT;
			case Yue::ShaderDataType::Int2:		return GL_INT;
			case Yue::ShaderDataType::Int3:		return GL_INT;
			case Yue::ShaderDataType::Int4:		return GL_INT;
			case Yue::ShaderDataType::Bool:		return GL_BOOL;
		}
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const {
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout) {
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)element.Offset);
					m_VertexBufferIndex++;

					break;
				}

				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribIPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						ShaderDataTypeToOpenGLBaseType(element.Type),
						layout.GetStride(),
						(const void*)element.Offset);
					m_VertexBufferIndex++;

					break;
				}
				
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (int i = 0;i < count;i++) {
						glEnableVertexAttribArray(m_VertexBufferIndex);
						glVertexAttribPointer(m_VertexBufferIndex,
							count,
							ShaderDataTypeToOpenGLBaseType(element.Type),
							element.Normalized ? GL_TRUE : GL_FALSE,
							layout.GetStride(),
							(const void*)(element.Offset + sizeof(float) * count * i));
						m_VertexBufferIndex++;
					}
					break;
				}
				default:
					break;
			}
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}