#pragma once

#include <chrono>

using namespace std;

class Time
{
public:

	static float GetDeltaTime() { return deltaTime; };
	static void CalculateDelta();

private:

	static float deltaTime;
	static chrono::steady_clock::time_point lastUpdate;

};

