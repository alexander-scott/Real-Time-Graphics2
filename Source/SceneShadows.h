#pragma once

#include "IScene.h"

#include "RenderTextureBuffer.h"

#include <vector>

class SceneShadows : public IScene
{
public:
	SceneShadows();

	bool Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) override;
	void Destroy() override;
	bool Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, float frameTime) override;

private:
	void ProcessInput(Input*, float) override;
	bool Draw(DX11Instance*, ShaderManager*) override;
	bool RenderSceneToTexture(DX11Instance* direct3D, ShaderManager* shaderManager);

	Light*					_light;
	RenderTextureBuffer*	_renderTextureBuffer;

	std::vector<Object*>	_objects;
	Object*					_cube;
};