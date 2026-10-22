#include "OpenGLTexture.h"
#include "Core/Assert.h"
#include <string>
#include <stb_image/stb_image.h>

namespace Yue {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):m_Path(path) {
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!data) {
			std::string errorMessage = "Failed to load texture:" + path;
			YUE_ERROR(errorMessage);
			m_IsLoaded = false;
			stbi_image_free(data);
			return;
		}

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}
		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		if (!internalFormat || !dataFormat) {
			YUE_ERROR("Texture format not supported: " + path);
			stbi_image_free(data);
			return;
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		// count mip levels based on the texture size
		int mipLevels = 1;
		int size = std::max(m_Width, m_Height);
		while(size>1){
			size /= 2;
			mipLevels++;
		}

		glTextureStorage2D(m_RendererID, mipLevels, internalFormat, m_Width, m_Height);

		// Trilinear filtering: linear between texels and linear between mip levels.
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// RGB rows are not necessarily 4-byte aligned. Without this, images
		// whose width * 3 is not a multiple of 4 can be skewed.
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		// Build levels 1..mipLevels-1 from the uploaded level 0.
		glGenerateTextureMipmap(m_RendererID);

		stbi_image_free(data);
		m_IsLoaded = true;

		YUE_CORE_INFO("Texture loaded: " + path + " (" + std::to_string(m_Width) + "x"
			+ std::to_string(m_Height) + ", " + std::to_string(mipLevels) + " mip levels)");
	}

	OpenGLTexture2D::~OpenGLTexture2D() {
		if (m_RendererID != 0) {
			glDeleteTextures(1, &m_RendererID);
			m_RendererID = 0;
		}
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_RendererID);
	}
}