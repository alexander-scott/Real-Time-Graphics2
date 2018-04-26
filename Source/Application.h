enum Scene 
{
	eSceneCombined,
	eSceneTerrainLOD,
	eSceneTerrainGeneration,
	eSceneSkeleton,
	eSceneGraphics
};

const Scene CURRENT_SCENE = Scene::eSceneCombined;

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
#include "SceneSkeleton.h"
#include "SceneGraphics.h"

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
	bool BuildSceneCombined(HWND hwnd, int screenWidth, int screenHeight);

	bool BuildSceneTerrainLOD(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneTerrainGeneration(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneSkeleton(HWND hwnd, int screenWidth, int screenHeight);
	bool BuildSceneGraphics(HWND hwnd, int screenWidth, int screenHeight);

	Input*			_input;
	DX11Instance*	_dx11Instance;
	ShaderManager*	_shaderManager;
	Timer*			_timer;

	IScene*			_scene;
};