#pragma once

#include <string>

struct EnemiesWave
{
	char bufEnemyName[256];
	std::string name = "Default_Name";
	int number = 0;
};

struct Wave
{
	float timerBeforeWave = 50.f;
	int EnemiesNumber = 0;
	std::vector<EnemiesWave> enemies;
};

struct WaveInfo
{
	float EnemySpawnPointX;
	float EnemySpawnPointZ;
	float EnemySpawnRange;
	int maxWave = 0;
	std::vector<Wave> wave;
};
