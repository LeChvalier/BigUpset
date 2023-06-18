#include "Economy.h"

void EconomySystem::Update()
{
	if (!timer.launchTimer)
	{
		timer.Start();
		timer.launchTimer = true;
	}

	if (timer.launchTimer && timer.GetDuration() > 1)
	{
		m_actualMoney += moneyPerSec;
		timer.launchTimer = false;
	}
}