#include "Timer.h"


void Timer::Start()
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}

float Timer::GetDuration()
{
	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - m_StartTime;
	return duration.count();
}