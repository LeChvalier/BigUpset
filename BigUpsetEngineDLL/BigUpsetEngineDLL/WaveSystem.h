#pragma once

#include "Timer.h"
#include "Entity.h"
#include "Logger.h"
#include "WaveInfo.h"

class Game;

class WaveSystem
{
public:
	WaveSystem();
	~WaveSystem();

	int waveRound = 0;
	int currentEnemyNumber = 0;

	bool waveStatut = false;
	bool gameFinish = false;

	void CreateWave(Scene* scene, WaveInfo waveInfo, std::list<UnitDefinition>& CharaDefinitions);
	void WavesUpdate(Scene* scene, WaveInfo waveInfo, std::list<UnitDefinition>& CharaDefinitions);
	void ResetWave();

	Timer m_timer;

private:
};