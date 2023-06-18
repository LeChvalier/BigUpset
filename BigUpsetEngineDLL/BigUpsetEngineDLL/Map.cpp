#include <iostream>

#include "ResourceManager.h"
#include "Objects.h"
#include "Logger.h"
#include "Entity.h"
#include "Maths.h"
#include "Scene.h"
#include "Map.h"

#include "Define.h"


//Map::Map(ResourceManager* resourceManager, const MapInfo& _mapInfo)
//{
//	//mapInfo = _mapInfo;
//
//	// Map is always a quad
//	model.mesh = resourceManager->AddMesh(QUAD);
//
//	model.texture = resourceManager->AddTexture(mapInfo.textureMap.c_str());
//
//	tiles = resourceManager->ParseMap(mapInfo.gameplayMap, tilesWidth, tilesHeight);
//}

float4 Map::GetTile(const float3& position) const
{
	if (!initialized)
	{
		Logger::Error("Map not initialized");
		return{ 0.f, 0.f, 0.f, 0.f };
	}
	int u = (position.x + scale.x / 2.f) * tilesWidth / scale.x;
	int v = (position.y + scale.z / 2.f) * tilesHeight / scale.z;

	Math::Clamp<int>(u, 0, tilesWidth);
	Math::Clamp<int>(v, 0, tilesHeight);

	unsigned int index = u + v * tilesWidth;

	return tiles[index];
}

GroundType Map::GetGroundType(const float3& position) const
{
	if (!initialized)
	{
		Logger::Error("Map not initialized");
		return GroundType::VOID;
	}

	float4 pixel = GetTile(position);

	// If the tile is out of range
	if (pixel.a == 0.f)
		return GroundType::VOID;

	if (std::max(std::max(pixel.e[0], pixel.e[1]), pixel.e[2]) == pixel.e[0])
		return GroundType::LAVA;
	else if (std::max(std::max(pixel.e[0], pixel.e[1]), pixel.e[2]) == pixel.e[1])
		return GroundType::GRASS;
	else if (std::max(std::max(pixel.e[0], pixel.e[1]), pixel.e[2]) == pixel.e[2])
		return GroundType::WATER;
	else
		return GroundType::VOID;
}

void Map::SpawnDecor(ResourceManager* resourceManager, std::list<DecorDefinition>& _decors, Scene* scene)
{
	srand(abs(seed));

	unsigned int tilesWidth = 0;
	unsigned int tilesHeight = 0;

	std::unique_ptr<float4[]> ObjectMap;

	ObjectMap = resourceManager->ParseMap(gameplayMap, tilesWidth, tilesHeight);

	// Non variants
	float sizefactor = scale.x / tilesWidth;
	float3 textureOffset = { -scale.x / 2, 0.f, -scale.z / 2.f };	// 5 is map width/height /2

	for (DecorDefinition& decor : _decors)
	{
		if (decor.m_id == decor_R_id)
			m_decors[0] = &decor;
		if (decor.m_id == decor_G_id)
			m_decors[1] = &decor;
		if (decor.m_id == decor_B_id)
			m_decors[2] = &decor;
	}

	if (m_decors[0]->m_name.empty() || m_decors[1]->m_name.empty() || m_decors[2]->m_name.empty())
	{
		Logger::Warn("Decor resources not loaded");
		return;
	}

	for (unsigned int i = 0; i < tilesWidth * tilesHeight; i++)
	{
		float4 pixel = ObjectMap[i];
		float2 uv = { (float)(i % tilesWidth), (float)i / (float)tilesHeight };

		float3 position = { uv.x, 0.f, uv.y };
		position = sizefactor * position + textureOffset;

		// RED
		if (pixel.e[0] > floorValue.x && rand() % 100 < spawnValue)
			scene->NewDecor(*m_decors[0], position, { 0.f, (float)(rand() % 360), 0.f });

		// GREEN
		else if (pixel.e[1] > floorValue.y && rand() % 100 < spawnValue)
			scene->NewDecor(*m_decors[1], position, { 0.f, (float)(rand() % 360), 0.f });

		// BLUE
		else if (pixel.e[2] > floorValue.z && rand() % 100 < spawnValue)
			scene->NewDecor(*m_decors[2], position, { 0.f, (float)(rand() % 360), 0.f });
	}
}