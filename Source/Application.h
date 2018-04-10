#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

enum Scene {
	eSceneTerrainLOD
};

const Scene CURRENT_SCENE = Scene::eSceneTerrainLOD;

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#include "Input.h"
#include "DX11Instance.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Timer.h"
#include "SceneTerrainLOD.h"

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Destroy();
	bool Update();

private:
	bool BuildSceneTerrainLOD(HWND hwnd, int screenWidth, int screenHeight);

	Input*			_input;
	DX11Instance*	_dx11Instance;
	ShaderManager*	_shaderManager;
	TextureManager* _textureManager;
	Timer*			_timer;

	IScene*			_scene;
};

#endif