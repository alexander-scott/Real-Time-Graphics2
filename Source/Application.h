#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#include "Input.h"
#include "DX11Instance.h"
#include "ShaderManager.h"
#include "Timer.h"
#include "FrameCounter.h"
#include "Scene.h"

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Update();

private:
	Input* m_Input;
	DX11Instance* m_Direct3D;
	ShaderManager* m_ShaderManager;
	Timer* m_Timer;
	FrameCounter* m_Fps;
	Scene* m_Zone;
};

#endif