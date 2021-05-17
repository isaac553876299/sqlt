#ifndef __TIMERS_H__
#define __TIMERS_H__

#include "SDL_timer.h"

class Timer
{
public:
	unsigned __int32 startTime;
	Timer() { Start(); }
	void Start() { startTime = SDL_GetTicks(); }
	unsigned __int32 ReadMs() const { return SDL_GetTicks() - startTime; }
	float ReadS() const { return float(SDL_GetTicks() - startTime) / 1000.0f; }
};

class PerfTimer
{
public:
	unsigned __int64 startTime;
	unsigned __int64 frequency;
	PerfTimer() { frequency = SDL_GetPerformanceFrequency(); Start(); }
	void Start() { startTime = SDL_GetPerformanceCounter(); }
	double ReadMs() const { return 1000.0 * (float(SDL_GetPerformanceCounter() - startTime) / float(frequency)); }
	unsigned __int64 ReadTicks() const { return SDL_GetPerformanceCounter() - startTime; }
};

#endif
