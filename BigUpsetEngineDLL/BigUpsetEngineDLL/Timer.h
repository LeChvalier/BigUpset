#pragma once
#include <chrono>


class Timer
{
private:
    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;

public:
    float deltaTime = 1.f / 60.f;
    
    bool launchTimer = false;

    void Start();
    float GetDuration();

};