#pragma once

#include "IScene.h"

class SceneCombined : public IScene
{
public:
	SceneCombined();

	bool Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) override;
	void Destroy() override;
	bool Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, float frameTime) override;

private:
	void ProcessInput(Input*, float) override;
	bool Draw(DX11Instance*, ShaderManager*) override;

	SkyDome*		_skyDome;
	Terrain*		_terrain;
	Skeleton*		_skeleton;
	Object*			_cube;

	bool			_wireFrame, _cellLines, _heightLocked;
};