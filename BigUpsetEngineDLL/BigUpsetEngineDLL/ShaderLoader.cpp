#include <glad/gl.h> 

#include "ShaderLoader.h"
#include "Logger.h"

GLuint ShaderLoader::CreateProgram(const char* VSString, const char* FSString)
{
	GLuint Program = glCreateProgram();

	GLuint VertexShader = CompileShader(GL_VERTEX_SHADER, VSString);
	GLuint FragmentShader = CompileShader(GL_FRAGMENT_SHADER, FSString);

	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragmentShader);

	glLinkProgram(Program);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return Program;
}

GLuint ShaderLoader::CompileShader(GLenum ShaderType, const char* ShaderStr)
{
	GLuint Shader = glCreateShader(ShaderType);

	glShaderSource(Shader, 1, &ShaderStr, nullptr);
	glCompileShader(Shader);

	GLint CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if (CompileStatus == GL_FALSE)
	{
		char Infolog[1024];
		glGetShaderInfoLog(Shader, 1024, nullptr, Infolog);

		std::string str = " Shader error: ";
		str += Infolog;
		Logger::Error(str);
	}

	return Shader;
}