#pragma once

#include "ResourceManager.h"

class Engine;
struct GLFWwindow;

class MousePicking
{
public:
	MousePicking(Renderer* _renderer, ResourceManager* _resourceManager);
	MousePicking() {};
	~MousePicking() {};

	unsigned int program = 0;

	// need to call after the scene render (cam update)
   /*void Render(const Camera& camera, const std::vector<UnitEntity>& charaEntities, const std::vector<UnitEntity>& buildEntities);*/

	void Init(GLFWwindow* window, float2 windowSize);

	void Update(Engine* engine, GLFWwindow* window, float2 windowSize, const Camera& camera, std::list<UnitDefinition>& charaDefs, std::list<UnitDefinition>& buildDefs, std::vector<UnitEntity>& charaEntities, std::vector<UnitEntity>& buildEntities, const Map& map, int& selectedEntity, int& selectedCreatableBuilding, const float2& EditorOffset = {0.f, 0.f});

	// Return Wordlpos at y = 0 from a screen position
	float3 FromScreenToWorldPos(const mat4& ViewProj, const float2& screenPos, const float2& screenSize);

	float3 lastClickedPos = { 0.f, 0.f, 0.f };

private:
	void RenderPickingFbo(const std::vector<UnitEntity>& entities, mat4 ViewProj);

	// Contains all the resources
	ResourceManager* resourceManager = nullptr;

	// Renders the scene
	Renderer* renderer = nullptr;

	std::vector<unsigned short> indices;
	std::vector<float3> indexed_vertices;
	std::vector<float2> indexed_uvs;
	std::vector<float3> indexed_normals;

	GLuint vertexbuffer = 0;
	GLuint elementbuffer = 0;

	float3 p0; // TODO: To remove
	float3 p1;

	// Draw picking render
	void DrawPickingScene(const mat4& viewProj, const std::vector<UnitEntity>& buildEntities, const std::vector<UnitEntity>& charaEntities);

	// return result of shader picking
	float ShaderPicking(const float2& pos, const float2& windowSize);

	int PickedUnit(const float3& pos, const std::vector<UnitEntity>& unitEntities);

	float m_accuracy = 0.5f;

	// For debug only
};
