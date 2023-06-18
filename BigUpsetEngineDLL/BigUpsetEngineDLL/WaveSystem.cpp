#include "WaveSystem.h"
#include "Scene.h"

WaveSystem::WaveSystem()
{
}

void WaveSystem::ResetWave()
{
	waveRound = 0;
	gameFinish = false;
}

void WaveSystem::WavesUpdate(Scene* scene, WaveInfo waveInfo, std::list<UnitDefinition>& CharaDefinitions)
{
	if (waveRound >= waveInfo.maxWave && currentEnemyNumber <= 0)
	{
		if (!gameFinish)
		{
			Logger::Info("Game win !");
			gameFinish = true;
		}

		return;
	}

	if (currentEnemyNumber <= 0 && !m_timer.launchTimer)
	{
		m_timer.Start();
		m_timer.launchTimer = true;
	}
	
	if (m_timer.launchTimer && m_timer.GetDuration() > waveInfo.wave[waveRound].timerBeforeWave)
	{
		m_timer.launchTimer = false;
		waveStatut = true;

		CreateWave(scene, waveInfo, CharaDefinitions);
	}

	int enemy = 0;

	for (int i = 0; i < scene->characterEntities.size(); i++)
		if (scene->characterEntities[i].definition.m_side == AISideType::ENEMY)
			enemy += 1;

	currentEnemyNumber = enemy;
}

void WaveSystem::CreateWave(Scene* scene, WaveInfo waveInfo, std::list<UnitDefinition>& CharaDefinitions)
{
	if (waveRound < waveInfo.maxWave)
	{
		int enemiesNb = 0;
		for (int i = 0; i < waveInfo.wave[waveRound].enemies.size(); i++)
		{
			enemiesNb += waveInfo.wave[waveRound].enemies[i].number;

			for (int j = 0; j < waveInfo.wave[waveRound].enemies[i].number; j++)
			{
				UnitDefinition& unit = GetUnitDefFromName(CharaDefinitions, waveInfo.wave[waveRound].enemies[i].name);
				float3 pos = { waveInfo.EnemySpawnPointX + std::rand() % (int)waveInfo.EnemySpawnRange, 0, waveInfo.EnemySpawnPointZ + std::rand() % (int)waveInfo.EnemySpawnRange };
				scene->NewUnit(unit, pos);
			}
		}
		
		waveRound += 1;
	}
	else
	{ 
		gameFinish = true;
	}
}

WaveSystem::~WaveSystem()
{
}
