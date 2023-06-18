#pragma once

#include <string>

#include "Timer.h"

class EconomySystem
{
public:
	EconomySystem() {};
	~EconomySystem() {};

	std::string name = "Default_name";
	int maxMoney = 10000;
	int startingMoney = 100;
	int moneyPerSec = 2;

	Timer timer;

	int GetMoney() { return m_actualMoney; };
	void AddMoney(int count) { m_actualMoney += count; };
	void SetMoney(int count) { m_actualMoney = count; };
	void RemoveMoney(int count) { m_actualMoney -= count; };
	void ResetMoney() { m_actualMoney = 0; };

	void Update();
private:
	int m_actualMoney = 0;
};