#pragma once

#include <memory>
#include <string>

#include "ResourceManager.h"
#include "SoundManager.h"
#include "WaveSystem.h"
#include "Renderer.h"
#include "Camera.h"
#include "Entity.h"
#include "Map.h"

#include "ParticlesSystem.h"

// This is the scene to display, have ids from the resource manager to the real loaded models/textures...
class Scene
{
public:
	Scene(Renderer* _renderer, ResourceManager* _resourceManager);
	~Scene() { map.Destroy(); };

	// Called once per frame
	void UpdateAndRender(std::list<UnitDefinition>& CharaDefinitions, std::list<UnitDefinition>& BuildDefinitions, const bool& inGame, const float2& windowSize, EconomySystem& economySystem);

	// Clean the scene objects
	void CleanScene();

	// The only one
	Camera camera;

	// Map
	Map map;

	// All buildings and Characters
	std::vector<UnitEntity> buildingEntities;
	std::vector<UnitEntity> characterEntities;
	// All decor objects
	std::vector<DecorEntity> decorEntities;
	// All particlesSystems
	std::vector<ParticlesSystem> particlesSystems;

	void NewUnit(UnitDefinition& definition, float3 pos);
	void NewDecor(DecorDefinition& definition, float3 pos = { 0.f, 0.f, 0.f }, float3 rotation = { 0.f, 0.f, 0.f });
	void NewParticlesSystem(ParticlesSystemDefinition def, const float3& _position = {0.f, 0.f, 0.f}, const float3& _rotation = { 0.f, 0.f, 0.f });

	// Used to render scene in editor/game
	const unsigned int GetCBO() const { return renderer->CBO; };

	// Contains all the Music/Sound
	SoundManager soundManager; // Sortir le path de l'ambient music

private:
	// Contains all the resources
	ResourceManager* resourceManager = nullptr;
	
	// Renders the scene
	Renderer* renderer;
	int IDcount = 0;

	//ParticulesSystem particleSystem;
};
