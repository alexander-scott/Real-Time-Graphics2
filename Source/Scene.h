#ifndef _ZONECLASS_H_
#define _ZONECLASS_H_

#include "DX11Instance.h"
#include "Input.h"
#include "ShaderManager.h"
#include "Timer.h"
#include "UserInterface.h"
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
	void Shutdown();
	bool Update(DX11Instance*, Input*, ShaderManager*, float, int);

private:
	void HandleMovementInput(Input*, float);
	bool Render(DX11Instance*, ShaderManager*);

private:
	UserInterface* m_UserInterface;
	Camera* m_Camera;
	Transform* m_Transform;
	Terrain* m_Terrain;
	bool m_displayUI, m_wireFrame;
};

#endif