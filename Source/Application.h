#pragma once

enum Scene
{
	eSceneCombined, // Contains terrain read in from file, controllable animated skeleton which can walk on terrain
	eSceneTerrainLOD, // Contains terrain read in from file and rendered with various LOD features
	eSceneTerrainGeneration, // Contains terrain generated using the circle hill algorithm
	eSceneVoxelTerrain, // Contains a voxel planet
	eSceneSkeleton, // Contains an animated skeleton
	eSceneDeferredShading, // Contains a single cube and light rendered using deferred shading
	eSceneShadows // Contains multiple objects which are lit and project shadows onto each other
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
#include "SceneDeferredShading.h"
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