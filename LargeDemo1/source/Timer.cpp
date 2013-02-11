#include "stdafx.h"
#include "Timer.h"


Timer::Timer()
{
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	Reset();
}


Timer::~Timer()
{
}

float Timer::GetTimeSeconds()
{
	LARGE_INTEGER endTime;
	QueryPerformanceCounter(&endTime);
	return static_cast<float>(endTime.QuadPart - _startTime.QuadPart) / _frequency;
}

void Timer::Reset()
{
	QueryPerformanceCounter(&_startTime);
}
