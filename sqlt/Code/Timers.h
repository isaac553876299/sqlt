#ifndef __TIMERS_H__
#define __TIMERS_H__

#include "SDL_timer.h"

class Timer
{
public:

	unsigned __int32 startTime;

	Timer() { Start(); }

	void Start() { startTime = SDL_GetTicks(); }

	inline unsigned __int32 ReadMs() const { return SDL_GetTicks() - startTime; }

	inline float ReadS() const { return float(SDL_GetTicks() - startTime) / 1000.0f; }

};

class PerfTimer
{
public:

	unsigned __int64 startTime;
	unsigned __int64 frequency;

	PerfTimer()
	{
		frequency = SDL_GetPerformanceFrequency();
		Start();
	}

	void Start() { startTime = SDL_GetPerformanceCounter(); }

	inline double ReadMs() const { return 1000.0 * (float(SDL_GetPerformanceCounter() - startTime) / float(frequency)); }

	inline unsigned __int64 ReadTicks() const { return SDL_GetPerformanceCounter() - startTime; }
};

#endif
