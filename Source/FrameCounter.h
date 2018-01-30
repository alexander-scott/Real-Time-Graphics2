#ifndef _FPSCLASS_H_
#define _FPSCLASS_H_

#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>

class FrameCounter
{
public:
	FrameCounter();
	FrameCounter(const FrameCounter&);
	~FrameCounter();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_fps, m_count;
	unsigned long m_startTime;
};

#endif