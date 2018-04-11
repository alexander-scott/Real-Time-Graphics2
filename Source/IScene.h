#pragma once

#include "DX11Instance.h"
#include "Input.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Timer.h"
#include "Camera.h"
#include "Light.h"
#include "Frustum.h"
#include "SkyDome.h"
#include "Terrain.h"

class IScene
{
public:
	IScene()
	{
		_camera = nullptr;
		_light = nullptr;
		_frustum = nullptr;
	}

	virtual bool Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth) = 0;
	virtual void Destroy() = 0;
	virtual bool Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, TextureManager* textureManager, float frameTime) = 0;

protected:
	virtual void ProcessInput(Input* Input, float frameTime) = 0;
	virtual bool Draw(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager) = 0;

protected:
	Camera*			_camera;
	Light*			_light;
	Frustum*		_frustum;
};