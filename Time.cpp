#include "Time.h"

float Time::deltaTime;
chrono::steady_clock::time_point Time::lastUpdate;

void Time::CalculateDelta()
{
	auto now = chrono::steady_clock::now();
	deltaTime = chrono::duration_cast<chrono::microseconds>(now - lastUpdate).count() / 100000.f;
	lastUpdate = now;
}
