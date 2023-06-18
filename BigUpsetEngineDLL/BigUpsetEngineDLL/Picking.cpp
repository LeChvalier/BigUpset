#include "BigUpsetEngine.h"
#include "ShaderLoader.h"
#include "Picking.h"
#include "Maths.h"

#include <imgui.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cassert>

#include <iomanip>

#include "Map.h"
#include "Scene.h"
#include "Debug.h"

// Shaders
// ==================================================
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
uniform vec4 PickingColor;

void main()
{
    color = PickingColor;
})GLSL";


MousePicking::MousePicking(Renderer* _renderer, ResourceManager* _resourceManager)
	: renderer(_renderer)
	, resourceManager(_resourceManager)
{

}

void MousePicking::Init(GLFWwindow* window, float2 windowSize)
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

static UnitEntity* FindUnitById(std::vector<UnitEntity>& charaEntities, int id)
{
	for (UnitEntity& unit : charaEntities)
	{
		if (unit.gameID == id)
		{
			return &unit;
		}
	}
	return nullptr;
}

bool InsideTheMap(const float3& pos, const float3& mapScale)
{
	if (pos.x < -mapScale.x / 2.f || pos.x > mapScale.x / 2.f
		|| -pos.z < -mapScale.z / 2.f || -pos.z> mapScale.z / 2.f)
		return false;

	return true;
}

void MousePicking::DrawPickingScene(const mat4& viewProj, const std::vector<UnitEntity>& buildEntities, const std::vector<UnitEntity>& charaEntities)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);

	// Only the positions are needed (not the UVs and normals)
	glEnableVertexAttribArray(0);

	RenderPickingFbo(buildEntities, viewProj);
	RenderPickingFbo(charaEntities, viewProj);

	glFlush();
	glFinish();
}

float MousePicking::ShaderPicking(const float2& pos, const float2& windowSize)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char data[4];

	glReadPixels(pos.x, windowSize.y - pos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	float pickedID =
		data[0] +
		data[1] * 256.f +
		data[2] * 256.f * 256.f;

	return pickedID;
}

int MousePicking::PickedUnit(const float3& pos, const std::vector<UnitEntity>& unitEntities)
{
	for (const UnitEntity& unit : unitEntities)
	{
		float3 vector = unit.position - pos;
		if (vector.Magnitude() < m_accuracy)
			return unit.gameID;
	}
	return -1;
}

void MousePicking::Update(Engine* engine, GLFWwindow* window, float2 windowSize, const Camera& camera, std::list<UnitDefinition>& charaDefs, std::list<UnitDefinition>& buildDefs, std::vector<UnitEntity>& charaEntities, std::vector<UnitEntity>& buildEntities, const Map& map, int& selectedEntity, int& selectedCreatableBuilding, const float2& editorOffset)
{
	mat4 viewProj = camera.projMat * camera.viewMat;

	// Inputs
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	xpos -= editorOffset.x;
	ypos -= editorOffset.y;

	// WorldPos
	float3 pos = FromScreenToWorldPos(Mat4::Inverse(viewProj), { (float)xpos, (float)ypos }, windowSize);

	// If clicked outside, doesn t care about it
	if (!InsideTheMap(pos, map.scale))
		return;

	// Select Entity
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
	{
		// Update LastPos
		lastClickedPos = pos;

		// Select character
		int pickedId = PickedUnit(pos, charaEntities);
		if (pickedId != -1)
		{
			for (auto& unit : charaEntities)
			{
				if (unit.gameID == pickedId)
				{
					engine->preview.ShowModel(unit.definition.model);
					selectedEntity = unit.gameID;
				}
			}
		}

		// Select building
		pickedId = PickedUnit(pos, buildEntities);
		if (pickedId != -1)
		{
			for (auto& build : buildEntities)
			{
				if (build.gameID == pickedId)
				{
					engine->preview.ShowModel(build.definition.model);
					//selectedCreatableBuilding = build.gameID;
					selectedEntity = build.gameID;
				}
			}
		}

		// Click in gamebutton
		if (selectedCreatableBuilding != -1)
		{
			for (UnitDefinition& def : buildDefs)
			{
				if (selectedCreatableBuilding == def.m_id)
				{
					engine->scene.NewUnit(def, pos);
					selectedCreatableBuilding = -1;
					break;
				}
			}
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
	{
		int pickedId = PickedUnit(pos, buildEntities);
		if (pickedId == -1)
			pickedId = PickedUnit(pos, charaEntities);

		if (pickedId == -1)
		{
			if (selectedEntity != -1)
			{
				for (UnitEntity& unit : charaEntities)
				{
					if (unit.gameID == selectedEntity)
					{
						unit.definition.movePos = pos;
						unit.definition.m_hasOrder = true;
						unit.definition.m_hasTarget = false;
					}
				}
				selectedCreatableBuilding = -1;
			}
		}
		else
		{
			if (selectedEntity != -1)
			{
				UnitEntity* selectedUnit = FindUnitById(charaEntities, selectedEntity);
				if (selectedUnit != nullptr)
					selectedUnit->script->OnClick(*selectedUnit, pos, pickedId, charaEntities, buildEntities);
			}
		}
	}
}

void MousePicking::RenderPickingFbo(const std::vector<UnitEntity>& entities, mat4 ViewProj)
{
	glBindFramebuffer(GL_FRAMEBUFFER, renderer->pickingFBO);

	unsigned int pickingColorLocation = glGetUniformLocation(program, "PickingColor");

	for (const UnitEntity& unit : entities)
	{
		glUniformMatrix4fv(glGetUniformLocation(program, "uViewProj"), 1, GL_FALSE, ViewProj.e);

		mat4 modelMatrix = Mat4::CreateTRS(unit.position, unit.rotation, unit.scale);

		assert(unit.definition.model.mesh != nullptr);
		assert(unit.definition.model.mesh->gpu != nullptr);

		int i = unit.gameID;

		// Convert "i", the integer mesh ID, into an RGB color
		int r = (i & 0x000000FF) >> 0;
		int g = (i & 0x0000FF00) >> 8;
		int b = (i & 0x00FF0000) >> 16;

		glUniform4f(pickingColorLocation, r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

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

float3 MousePicking::FromScreenToWorldPos(const mat4& invViewProj, const float2& screenPos, const float2& screenSize)
{
	// Between -1 and 1
	float3 ndc = { (screenPos.x / screenSize.x) * 2.f - 1.f, (screenPos.y / screenSize.y) * 2.f - 1.f, 0.f };

	// The two points on the near/far plane
	// Inverted y beaucause of the inverted axis
	float3 ndcNear = { ndc.x, -ndc.y, -1.f };
	float3 ndcFar = { ndc.x, -ndc.y,  1.f };
	p0 = Math::FromScreenViewPointToWorldPos(invViewProj, ndcNear);
	p1 = Math::FromScreenViewPointToWorldPos(invViewProj, ndcFar);

	// Normal vector
	float3 n = p1 - p0;

	// Factor from p0 to pw
	assert(n.y != 0);
	// Easy equation
	float a = -p0.y / n.y;

	float3 pickedWordPos = p0 + n * a;

	return pickedWordPos;
}
