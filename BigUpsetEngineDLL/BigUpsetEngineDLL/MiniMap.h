#pragma once

#include "ResourceManager.h"

class Engine;
struct GLFWwindow;
class UnitEntity;
struct Map;

class MiniMap
{
public:
	MiniMap(Renderer* _renderer, ResourceManager* _resourceManager);
	MiniMap() = default;
	~MiniMap() = default;

	unsigned int program = 0;

    // The only one
    Camera camera;

	void Init(GLFWwindow* window, float2 windowSize);

	void Update(Map& map, const std::vector<UnitEntity>& charaEntities, const std::vector<UnitEntity>& buildEntities, Engine* engine, GLFWwindow* window, float2 windowSize);
    const unsigned int GetCBO() const { return renderer->miniMapCBO; };
private:
	void RenderMiniMapFBO(const std::vector<UnitEntity>& entities, mat4 ViewProj);

    // Contains all the resources
    ResourceManager* resourceManager = nullptr;

    // Renders the scene
    Renderer* renderer = nullptr;

    std::vector<unsigned short> indices;
    std::vector<float3> indexed_vertices;
    std::vector<float2> indexed_uvs;
    std::vector<float3> indexed_normals;

    GLuint vertexbuffer;
    GLuint elementbuffer;
};