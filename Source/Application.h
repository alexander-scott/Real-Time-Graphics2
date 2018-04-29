#pragma once

enum Scene
{
	eSceneCombined,
	eSceneTerrainLOD,
	eSceneTerrainGeneration,
	eSceneVoxelTerrain,
	eSceneSkeleton,
	eSceneDeferred,
	eSceneShadows
};

const Scene CURRENT_SCENE = Scene::eSceneVoxelTerrain;

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

#include "Input.h"
#include "DX11Instance.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Timer.h"

#include "SceneCombined.h"
#include "SceneTerrainLOD.h"
#include "SceneTerrainGeneration.h"
#include "SceneVoxelTerrain.h"
#include "SceneSkeleton.h"
#include "SceneDeferredLighting.h"
#include "SceneShadows.h"

class Application
{
public:
	Application();
	~Application();

	bool Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
	void Destroy();
	bool Update();

private:
	bool BuildSceneCombined(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneTerrainLOD(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneTerrainGeneration(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneVoxelTerrain(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneSkeleton(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneDeferred(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneShadows(HWND hwnd, int screenWidth, int screenHeight);

	Input*			_input;
	DX11Instance*	_dx11Instance;
	ShaderManager*	_shaderManager;
	Timer*			_timer;

	IScene*			_scene;
};