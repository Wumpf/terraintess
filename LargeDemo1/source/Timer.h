#pragma once
class Timer
{
public:
	Timer();
	~Timer();

	void Reset();
	float GetTimeSeconds();

private:
	__int64 _frequency;
	LARGE_INTEGER _startTime;
};

