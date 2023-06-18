#pragma once

#include <string>
#include <memory>

struct DecorDefinition;
class ResourceManager;
class Scene;

enum class GroundType
{
	LAVA,
	WATER,
	GRASS,
	VOID,
};

enum class LandscapeType
{
	ROCK,
	REEF,
	TREE,
	VOID,
};

struct Map
{
	// Functions
	void Destroy()
	{
		if (!initialized)
			return;

		tiles.release();
	};

	GroundType GetGroundType(const float3& position) const;

	float4 GetTile(const float3& position) const;

	void SpawnDecor(ResourceManager* resourceManager, std::list<DecorDefinition>& _decors, Scene* scene);

	// Data
	// TODO deleted
	Model model = { new Mesh(), new Texture()};

	// Textures
	std::string objectMap = "";
	std::string gameplayMap = "";

	// Transform
	float3 position = {0.f, 0.f, 0.f};
	float3 rotation = { 0.f, 0.f, 0.f };
	float3 scale = { 0.f, 0.f, 0.f };

	// Spawn parameters
	float3 floorValue = {};
	float spawnValue = 0;

	// Decors
	int decor_R_id = 0;
	int decor_G_id = 0;
	int decor_B_id = 0;

	// Noise texture
	std::unique_ptr<float4[]> tiles;

	unsigned int tilesWidth = 0, tilesHeight = 0;

	bool initialized = false;

	DecorDefinition* m_decors[3];

	int seed = 0;
};