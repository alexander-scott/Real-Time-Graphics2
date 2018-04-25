#pragma once

#include "IScene.h"

#include "Terrain.h"

class SceneTerrainLOD : public IScene
{
public:
	SceneTerrainLOD();
	SceneTerrainLOD(const SceneTerrainLOD&);

	bool Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) override;
	void Destroy() override;
	bool Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, float frameTime) override;

private:
	void ProcessInput(Input*, float) override;
	bool Draw(DX11Instance*, ShaderManager*, TextureManager*) override;

	SkyDome*		_skyDome;
	Terrain*		_terrain;

	bool			_wireFrame, _cellLines, _heightLocked;
};