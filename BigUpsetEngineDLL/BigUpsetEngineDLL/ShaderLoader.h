#pragma once

typedef unsigned int GLenum;
typedef unsigned int GLuint;


class ShaderLoader
{
public:
	GLuint CreateProgram(const char* VSString, const char* FSString);

private:
	// Compile shader and return GLuint pointer on
	GLuint CompileShader(GLenum ShaderType, const char* ShaderStr);
};