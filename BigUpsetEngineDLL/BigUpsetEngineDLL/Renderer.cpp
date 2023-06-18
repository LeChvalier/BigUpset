#include <stb_image.h>
#include <glad/gl.h>
#include <memory>

#include "TextureLoader.h"
#include "Renderer.h"
#include "Logger.h"

// Shaders
// ==================================================
static const char* vertexShader = R"GLSL(
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uViewProj;

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
}
)GLSL";

static const char* gameVertexShader = R"GLSL(
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = vec4(aPosition, 1.0);
}
)GLSL";

static const char* fragmentShader = R"GLSL(
#version 450 core

in vec2 vUV;

uniform sampler2D uDiffuseTexture;

out vec4 oColor;

void main()
{
    oColor = texture(uDiffuseTexture, vUV);

	if(oColor.a < 0.1)
		discard;
}
)GLSL";


Renderer::Renderer()
{
	// Compile shaders and shader program
	m_program = shaderLoader->CreateProgram(vertexShader, fragmentShader);
	m_gameProgram = shaderLoader->CreateProgram(gameVertexShader, fragmentShader);

	// Link shader program
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "uColorTexture"), 0);
	glUseProgram(m_gameProgram);
	glUniform1i(glGetUniformLocation(m_gameProgram, "uColorTexture"), 0);
	glUseProgram(0);

	// Initiate FBO to render in Game
	CreateFBO(FBO, CBO, RBO);

	// Initate FBO in preview
	CreateFBO(previewFBO, previewCBO, previewRBO);

	CreateFBO(pickingFBO, pickingCBO, pickingRBO);

	CreateFBO(miniMapFBO, miniMapCBO, miniMapRBO);
}

Renderer::~Renderer()
{
	// Deleted gl resources in scene
}

GPUMesh* Renderer::CreateMesh(const Mesh& mesh)
{
	GPUMesh* result = new GPUMesh();

	// Fill up local vector with ordered vertices with indices
	std::vector<Vertex> vertexBuffer;
	for (int i = 0; i < mesh.indices.size(); i++)
		vertexBuffer.push_back(mesh.vertices[mesh.indices[i]]);

	// Create VBO mesh
	result->verticesNumber = (int)vertexBuffer.size();
	glGenBuffers(1, &result->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, result->VBO);
	glBufferData(GL_ARRAY_BUFFER, result->verticesNumber * sizeof(Vertex), vertexBuffer.data(), GL_STATIC_DRAW);

	// Create VAO mesh
	glGenVertexArrays(1, &result->VAO);
	glBindVertexArray(result->VAO);

	// Set space between datas
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return result;
}

GPUTexture* Renderer::CreateTexture(const Texture& texture)
{
	GPUTexture* textureGPU = new GPUTexture();

	// Create Texture
	glGenTextures(1, &textureGPU->texture);
	glBindTexture(GL_TEXTURE_2D, textureGPU->texture);

	UploadTexture(texture.path.c_str(), 0);
	// TODO: Handle error (return a bool and handle it)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureGPU;
}

void Renderer::StartDraw(const mat4& proj, const mat4& view, int _fbo, float2 screenSize, float4 cleanColor)
{

	UpdateRBOs(screenSize);
	// First pass, render the 3d scene with all elements
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	glEnable(GL_DEPTH_TEST);
	glClearColor(cleanColor.r, cleanColor.g, cleanColor.b, cleanColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);

	mat4 ViewProj = proj * view;
	glUniformMatrix4fv(glGetUniformLocation(m_program, "uViewProj"), 1, GL_FALSE, ViewProj.e);
}

void Renderer::EndDraw()
{
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawObject(const mat4& model, const Model& object)
{
	assert(object.mesh != nullptr);
	assert(object.mesh->gpu != nullptr);
	assert(object.texture != nullptr);
	assert(object.texture->gpu != nullptr);

	// Send uniforms
	{
		glUniformMatrix4fv(glGetUniformLocation(m_program, "uModel"), 1, GL_FALSE, model.e);
	}

	// Bind material (textures)
	{
		GPUTexture* diffuseTextureGPU = (GPUTexture*)object.texture->gpu;
		glBindTexture(GL_TEXTURE_2D, diffuseTextureGPU->texture);
	}

	// Draw mesh
	{
		GPUMesh* meshGPU = (GPUMesh*)object.mesh->gpu;
		glBindVertexArray(meshGPU->VAO);
		glDrawArrays(GL_TRIANGLES, 0, meshGPU->verticesNumber);
	}
}

void Renderer::CreateFBO(unsigned int& _FBO, unsigned int& _CBO, unsigned int& _RBO)
{
	// FBO
	glGenFramebuffers(1, &_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	// CBO
	glGenTextures(1, &_CBO);
	glBindTexture(GL_TEXTURE_2D, _CBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1200, 800, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _CBO, 0);

	// RBO
	glGenRenderbuffers(1, &_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, _RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 800);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _RBO);

	// Check errors
	auto FBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (FBOStatus != GL_FRAMEBUFFER_COMPLETE)
		Logger::Error("Framebuffer error: " + std::to_string(FBOStatus));
}

void Renderer::UpdateRBOs(float2 size)
{
	UpdateCBORBO(RBO, CBO, size);
	UpdateCBORBO(miniMapRBO, miniMapCBO, size);
	UpdateCBORBO(pickingRBO, pickingCBO, size);
	UpdateCBORBO(previewRBO, previewCBO, size);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Renderer::UpdateCBORBO(unsigned int& RBO, unsigned int& CBO, float2 size)
{
	glBindTexture(GL_TEXTURE_2D, CBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
}

void Renderer::RenderQuad()
{
	// Init quad
	static unsigned int quadVBO;
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	// VAO
	static GLuint quadVAO = 0;
	if (quadVAO == 0)
	{
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		// VBO
		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

		// Set spaces between datas
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	// Draw Quad
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}