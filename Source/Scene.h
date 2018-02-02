#ifndef _ZONECLASS_H_
#define _ZONECLASS_H_

#include "DX11Instance.h"
#include "Input.h"
#include "ShaderManager.h"
#include "Timer.h"
#include "Camera.h"
#include "Transform.h"
#include "Terrain.h"

class Scene
{
public:
	Scene();
	Scene(const Scene&);
	~Scene();

	bool Initialize(DX11Instance*, HWND, int, int, float);
	void Destroy();
	bool Update(DX11Instance*, Input*, ShaderManager*, float);

private:
	void HandleMovementInput(Input*, float);
	bool Render(DX11Instance*, ShaderManager*);

private:
	Camera* m_Camera;
	Transform* m_Transform;
	Terrain* m_Terrain;
	bool m_wireFrame;
};

#endif