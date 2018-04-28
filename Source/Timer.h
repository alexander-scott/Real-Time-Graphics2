#pragma once

#include <windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	bool Initialize();
	void Update();

	float GetTime();

	void StartTimer();
	void StopTimer();
	int GetTiming();

private:
	float		_frequency;
	INT64		_startTime;
	float		_frameTime;
	INT64		_beginTime, _endTime;
};