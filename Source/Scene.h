#ifndef _ZONECLASS_H_
#define _ZONECLASS_H_

#include "DX11Instance.h"
#include "Input.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Timer.h"
#include "Camera.h"
#include "Light.h"
#include "Terrain.h"

class Scene
{
public:
	Scene();
	Scene(const Scene&);
	~Scene();

	bool Initialize(DX11Instance*, HWND, int, int, float);
	void Destroy();
	bool Update(DX11Instance*, Input*,ShaderManager*, TextureManager*, float);

private:
	void HandleMovementInput(Input*, float);
	bool Render(DX11Instance*, ShaderManager*, TextureManager*);

private:
	Camera*			_camera;
	Light*			_light;
	Terrain*		_terrain;
	bool			_wireFrame;
};

#endif