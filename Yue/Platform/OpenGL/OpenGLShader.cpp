#include "OpenGLShader.h"
#include "OpenGL.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace Yue {
	static std::string ReadFile(const std::string& filepath) {
		std::ifstream in(filepath);
		std::stringstream ss;
		ss << in.rdbuf();
		return ss.str();
	}

	GLenum OpenGLShader::CompileShader(GLenum type, const std::string& source) {
		GLenum shader = glCreateShader(type);

		const char* src = source.c_str();

		glShaderSource(shader, 1, &src, nullptr);

		glCompileShader(shader);

		return shader;
	}

	GLenum OpenGLShader::CreateProgram(GLenum vertexShader, GLenum fragmentShader) {
		GLuint program = glCreateProgram();

		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

		glLinkProgram(program);

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