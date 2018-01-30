#include "FrameCounter.h"

FrameCounter::FrameCounter()
{
}

FrameCounter::FrameCounter(const FrameCounter& other)
{
}

FrameCounter::~FrameCounter()
{
}

void FrameCounter::Initialize()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
}

void FrameCounter::Frame()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

int FrameCounter::GetFps()
{
	return m_fps;
}