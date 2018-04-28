#pragma once

#include "IScene.h"

#include "NewCube.h"
#include "OrthoWindow.h"
#include "DeferredBuffers.h"

class SceneDeferredLighting : public IScene
{
public:
	SceneDeferredLighting();
	SceneDeferredLighting(const SceneDeferredLighting&);

	bool Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) override;
	void Destroy() override;
	bool Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, float frameTime) override;

private:
	void ProcessInput(Input*, float) override;
	bool Draw(DX11Instance*, ShaderManager*, TextureManager*) override;
	bool RenderSceneToTexture(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager);

	Light* _light;
	NewCube* _cube;
	OrthoWindow* _window;
	DeferredBuffers* _deferredBuffers;
};

