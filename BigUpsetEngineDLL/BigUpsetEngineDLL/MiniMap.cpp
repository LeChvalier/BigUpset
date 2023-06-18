#include "BigUpsetEngine.h"
#include "ShaderLoader.h"
#include "MiniMap.h"
#include "Scene.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

// Shaders
// ===============================================
static const char* vertexShader = R"GLSL(
#version 450 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uViewProj;

void main()
{
    gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
})GLSL";

static const char* fragmentShader = R"GLSL(
#version 450 core

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform vec4 EntityColor;

void main()
{
    color = EntityColor;
})GLSL";

MiniMap::MiniMap(Renderer* _renderer, ResourceManager* _resourceManager)
    :renderer(_renderer)
    , resourceManager(_resourceManager)
{
	camera.yaw = 0.f;
	camera.roll = 0.f;
	camera.pitch = -1.571f;
	camera.near = 0.1f;
	camera.far = 500.f;
	camera.position.x = 0.f;
	camera.position.y = 28.5f;
	camera.position.z = 0.f;
}

void MiniMap::Init(GLFWwindow* window, float2 windowSize)
{
	ShaderLoader loader;
	program = loader.CreateProgram(vertexShader, fragmentShader);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, windowSize.x / 2.f, windowSize.y / 2.f);

	glUseProgram(program);
	//glUniform1i(glGetUniformLocation(program, "uColorTexture"), 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void MiniMap::Update(Map& map, const std::vector<UnitEntity>& charaEntities, const std::vector<UnitEntity>& buildEntities, Engine* engine, GLFWwindow* window, float2 windowSize)
{
	//renderer->UpdateCBORBO(renderer->miniMapRBO, renderer->miniMapRBO, size);

	camera.Update(windowSize.x, windowSize.y);
	renderer->StartDraw(camera.projMat, camera.viewMat, renderer->miniMapFBO, windowSize);

	mat4 ViewProj = camera.projMat * camera.viewMat;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderer->DrawObject(Mat4::CreateTRS(map.position, map.rotation, map.scale), map.model);
	glUseProgram(program);

	// Only the positions are needed (not the UVs and normals)
	glEnableVertexAttribArray(0);
	RenderMiniMapFBO(buildEntities, ViewProj);
	RenderMiniMapFBO(charaEntities, ViewProj);

	renderer->EndDraw();
}

void MiniMap::RenderMiniMapFBO(const std::vector<UnitEntity>& entities, mat4 ViewProj)
{
	unsigned int colorLocation = glGetUniformLocation(program, "EntityColor");

	for (const UnitEntity& unit : entities)
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "uViewProj"), 1, GL_FALSE, ViewProj.e);

		mat4 modelMatrix = Mat4::CreateTRS(unit.position, unit.rotation, unit.scale);

		assert(unit.definition.model.mesh != nullptr);
		assert(unit.definition.model.mesh->gpu != nullptr);

		int r = 0;
		int g = 0;
		int b = 0;

		if (unit.definition.m_side == 0)
		{
			r = 0;
			g = 0;
			b = 255;
		}
		else if (unit.definition.m_side == 1)
		{
			r = 255;
			g = 0;
			b = 0;
		}
		else if (unit.definition.m_side == 2)
		{
			r = 100;
			g = 100;
			b = 100;
		}


		glUniform4f(colorLocation, r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

		// Send uniforms
		{
			glUniformMatrix4fv(glGetUniformLocation(program, "uModel"), 1, GL_FALSE, modelMatrix.e);
		}

		// Draw mesh
		{
			GPUMesh* meshGPU = (GPUMesh*)unit.definition.model.mesh->gpu;
			glBindVertexArray(meshGPU->VAO);
			glDrawArrays(GL_TRIANGLES, 0, meshGPU->verticesNumber);
		}
	}

}