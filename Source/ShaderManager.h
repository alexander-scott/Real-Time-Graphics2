#ifndef _SHADERMANAGERCLASS_H_
#define _SHADERMANAGERCLASS_H_

#include "DX11Instance.h"
#include "ColourShader.h"
#include "TextureShader.h"
#include "LightShader.h"
#include "SkydomeShader.h"
#include "TerrainShader.h"

class ShaderManager
{
public:
	ShaderManager();
	ShaderManager(const ShaderManager&);
	~ShaderManager();

	bool Initialize(ID3D11Device*, HWND);
	void Destroy();

	bool RenderColourShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
	bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	bool RenderLightShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	bool RenderSkyDomeShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4);
	bool RenderTerrainShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:
	ColourShader*		_colourShader;
	TextureShader*		_textureShader;
	LightShader*		_lightShader;
	SkyDomeShader*		_skydomeShader;
	TerrainShader*		_terrainShader;
};

#endif