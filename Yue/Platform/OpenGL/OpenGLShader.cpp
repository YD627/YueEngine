#include "OpenGLShader.h"
#include "OpenGL.h"
#include "Core/Assert.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace Yue {
	static std::string ReadFile(const std::string& filepath) {
		std::ifstream in(filepath);

		if (!in.is_open()) {
			YUE_CORE_ERROR("Failed to open shader file!");
			return {};
		}

		std::stringstream ss;
		ss << in.rdbuf();
		return ss.str();
	}

	GLenum OpenGLShader::CompileShader(GLenum type, const std::string& source) {
		GLenum shader = glCreateShader(type);

		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		GLint success = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success != GL_TRUE) {
			GLint logLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

			std::string infoLog(static_cast<size_t>(logLength>1?logLength:1),'\0');

			GLsizei written = 0;
			glGetShaderInfoLog(shader, static_cast<GLsizei>(infoLog.size()), &written, infoLog.data());

			if (type == GL_VERTEX_SHADER){
				YUE_CORE_ERROR("Vertex shader compilation failed: " + infoLog);
			}
			else if (type == GL_FRAGMENT_SHADER) {
				YUE_CORE_ERROR("Fragment shader compilation failed: " + infoLog);
			}

			glDeleteShader(shader);
			return 0;
		}

		return shader;
	}

	GLenum OpenGLShader::CreateProgram(GLenum vertexShader, GLenum fragmentShader) {
		if (vertexShader == 0 || fragmentShader == 0) {
			YUE_CORE_ERROR("Shader program creation failed due to shader compilation error.");
			if (vertexShader != 0) glDeleteShader(vertexShader);
			if (fragmentShader != 0) glDeleteShader(fragmentShader);
			return 0;
		}
		GLuint program = glCreateProgram();

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint success = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (success != GL_TRUE){
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

			std::string infoLog(static_cast<size_t>(logLength > 1 ? logLength : 1), '\0');

			GLsizei written = 0;
			glGetProgramInfoLog(program, static_cast<GLsizei>(infoLog.size()), &written, infoLog.data());

			YUE_CORE_ERROR("Shader program linking failed: " + infoLog);

			glDeleteProgram(program);
			program = 0;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return program;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) :m_Name(name) {
		auto vertexSource = ReadFile(vertexSrc);
		auto fragmentSource = ReadFile(fragmentSrc);

		GLenum vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
		GLenum fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

		m_RendererID = CreateProgram(vertexShader, fragmentShader);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const {
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const {
		glUseProgram(0);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& matrix) {
		GLuint location = glGetUniformLocation(m_RendererID, name.c_str());

		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}