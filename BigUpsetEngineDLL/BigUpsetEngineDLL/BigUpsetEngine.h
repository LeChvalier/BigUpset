#pragma once

#include <list>

#include "Scene.h"
#include "Renderer.h"
#include "Entity.h"
#include "Preview.h"

class Engine
{
public:
	Engine();
	Engine(bool isGame);
	~Engine();

public:
	void Display(std::list<UnitDefinition>& charaters, std::list<UnitDefinition>& buildings, const bool& inGame, const float2& windowSize, EconomySystem& economySystem);
	Renderer renderer = {};
	ResourceManager resourceManager;
	Scene scene;
	Preview preview;
};
