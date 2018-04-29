#pragma once

#include "DX11Instance.h"

#include <map>

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

	bool RenderColourShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix);
	bool RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);
	bool RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);
	bool RenderSkyDomeShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor);
	bool RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap,
		ID3D11ShaderResourceView* normalMap2, ID3D11ShaderResourceView* normalMap3,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

	bool RenderDeferredShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture);
	bool RenderDeferredLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colourTexture, ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection);

	bool RenderDepthShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix);
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