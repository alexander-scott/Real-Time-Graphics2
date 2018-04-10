#include "IScene.h"

class SceneTerrainLOD : public IScene
{
public:
	SceneTerrainLOD();
	SceneTerrainLOD(const SceneTerrainLOD&);

	bool Initialize(DX11Instance*, HWND, int, int, float) override;
	void Destroy() override;
	bool Update(DX11Instance*, Input*,ShaderManager*, TextureManager*, float) override;

private:
	void ProcessInput(Input*, float) override;
	bool Draw(DX11Instance*, ShaderManager*, TextureManager*) override;

private:
	Camera*			_camera;
	Light*			_light;
	Frustum*		_frustum;
	SkyDome*		_skyDome;
	Terrain*		_terrain;

	bool			_wireFrame, _cellLines, _heightLocked;
};