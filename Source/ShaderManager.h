#pragma once
#include "DX11Instance.h"
#include "ColourShader.h"
#include "TextureShader.h"
#include "LightShader.h"
#include "SkydomeShader.h"
#include "TerrainShader.h"

#include "DeferredShader.h"
#include "DeferredLightShader.h"

#include "DepthShader.h"
#include "ShadowShader.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool Initialize(ID3D11Device*, HWND);
	void Destroy();

	bool RenderColourShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
	bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	bool RenderLightShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	bool RenderSkyDomeShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4);
	bool RenderTerrainShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

	bool RenderDeferredShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
	bool RenderDeferredLightShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3);

	bool RenderDepthShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
	bool RenderShadowShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition,
		XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor);

private:
	ColourShader*			_colourShader;
	TextureShader*			_textureShader;
	LightShader*			_lightShader;
	SkyDomeShader*			_skydomeShader;
	TerrainShader*			_terrainShader;

	DeferredShader*			_deferredShader;
	DeferredLightShader*	_deferredLightShader;

	DepthShader*			_depthShader;
	ShadowShader*			_shadowShader;
};