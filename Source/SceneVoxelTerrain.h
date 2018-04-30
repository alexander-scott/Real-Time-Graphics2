#pragma once

#include "IScene.h"

#include <vector>

#include "VoxelTerrain.h"

class SceneVoxelTerrain : public IScene
{
public:
	SceneVoxelTerrain();

	bool Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) override;
	void Destroy() override;
	bool Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, float frameTime) override;

private:
	void ProcessInput(Input*, float) override;
	bool Draw(DX11Instance*, ShaderManager*) override;

	Light*					_light;

	Object*					_voxel;
	VoxelTerrain*			_voxelTerrain;
};