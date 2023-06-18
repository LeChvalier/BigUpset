#pragma once

#include <vector>
#include <list>

#include "SoundInfo.h"
#include "WaveInfo.h"
#include "WaveSystem.h"
#include "Picking.h"
#include "MiniMap.h"
#include "Economy.h"

struct GLFWwindow;

struct UnitDefinition;
struct DecorDefinition;
class Engine;

class GameInfo
{
public:
	int selectedEntity = -1;
	int selectedCreatableBuilding = -1;
};

class Game
{
public:
	Game(Engine* _engine, GLFWwindow* window);
	~Game() = default;

	void Init(GLFWwindow* window, float2 windowSize);

	float deltaTime = 1.f / 60.f;
	bool InGame = false;

	int m_numberIdCharacter = 0;
	int m_numberIdBuildings = 0;
	int m_numberIdDecors = 0;
	int m_numberIdParticles = 0;

	Engine* engine;

	// In engine (creation and destruction)
	//std::vector<std::unique_ptr<Entity>> m_entities;

	// List of references for quick access (WARNING: keep in sync)
	//std::vector<GameEntity*> m_gameEntities;
	// std::list for stable pointers
	std::list<UnitDefinition> m_charactersTemplate;
	std::list<UnitDefinition> m_buildingsTemplate;
	std::list<DecorDefinition> m_decorsTemplate;
	std::list<ParticlesSystemDefinition> m_particlesTemplate;

	EconomySystem economySystem;

	SoundInfo soundInfo;
	WaveInfo waveInfo;
	GameInfo gameInfo;

	MousePicking mousePicking;
	MiniMap miniMap;

	WaveSystem wavesystem;
	
	void uiUpdate(const float2& windowSize);
	void overlayEntityUpdate();
	void overlayGlobalInfoUpdate();
	void miniMapUpdate(const float2& windowSize);

	void newCharacterTemplate();
	void newBuildingsTemplate();
	void newDecorTemplate();
	void newParticleTemplate();

	void CreateGame();
	void gameUpdate(GLFWwindow* window);
	void ResetGame();

	void camMovement(GLFWwindow* window);

	std::string name = "Game name";

private:
	
	bool m_overlayOpen = true;
	//int tilesWidth = 0;
	//int tilesHeight = 0;
	//std::unique_ptr<float4[]> minimap;

	//Texture* minimapTexture = nullptr;


};
