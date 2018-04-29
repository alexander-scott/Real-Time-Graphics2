#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	_colourShader = nullptr;
	_textureShader = nullptr;
	_lightShader = nullptr;
	_skydomeShader = nullptr;
	_terrainShader = nullptr;
}

ShaderManager::~ShaderManager()
{
}

bool ShaderManager::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Create the Colour shader object.
	_colourShader = new ColourShader;
	if(!_colourShader)
	{
		return false;
	}

	// Initialize the Colour shader object.
	result = _colourShader->Initialize(device, hwnd, L"Source/Shaders/ColourPixelShader.hlsl", L"Source/Shaders/ColourVertexShader.hlsl");
	if(!result)
	{
		return false;
	}

	// Create the TargaTexture shader object.
	_textureShader = new TextureShader;
	if (!_textureShader)
	{
		return false;
	}

	// Initialize the TargaTexture shader object.
	result = _textureShader->Initialize(device, hwnd, L"Source/Shaders/TexturePixelShader.hlsl", L"Source/Shaders/TextureVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	// Create the light shader object.
	_lightShader = new LightShader;
	if (!_lightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = _lightShader->Initialize(device, hwnd, L"Source/Shaders/LightPixelShader.hlsl", L"Source/Shaders/LightVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	// Create the sky dome shader object.
	_skydomeShader = new SkyDomeShader;
	if (!_skydomeShader)
	{
		return false;
	}

	// Initialize the sky dome shader object.
	result = _skydomeShader->Initialize(device, hwnd, L"Source/Shaders/SkydomePixelShader.hlsl", L"Source/Shaders/SkydomeVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	// Create the terrain shader object.
	_terrainShader = new TerrainShader;
	if (!_terrainShader)
	{
		return false;
	}

	// Initialize the terrain shader object.
	result = _terrainShader->Initialize(device, hwnd, L"Source/Shaders/TerrainPixelShader.hlsl", L"Source/Shaders/TerrainVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	// Create the deferred shader object.
	_deferredShader = new DeferredShader;
	if (!_deferredShader)
	{
		return false;
	}

	// Initialize the deferred shader object.
	result = _deferredShader->Initialize(device, hwnd, L"Source/Shaders/DeferredPixelShader.hlsl", L"Source/Shaders/DeferredVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	// Create the deferred light shader object.
	_deferredLightShader = new DeferredLightShader;
	if (!_deferredLightShader)
	{
		return false;
	}

	// Initialize the deferred light shader object.
	result = _deferredLightShader->Initialize(device, hwnd, L"Source/Shaders/DeferredLightPixelShader.hlsl", L"Source/Shaders/DeferredLightVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	// Create the depth shader object.
	_depthShader = new DepthShader;
	if (!_depthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	result = _depthShader->Initialize(device, hwnd, L"Source/Shaders/DepthPixelShader.hlsl", L"Source/Shaders/DepthVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	// Create the shadow shader object.
	_shadowShader = new ShadowShader;
	if (!_shadowShader)
	{
		return false;
	}

	// Initialize the shadow shader object.
	result = _shadowShader->Initialize(device, hwnd, L"Source/Shaders/ShadowPixelShader.hlsl", L"Source/Shaders/ShadowVertexShader.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void ShaderManager::Destroy()
{
	// Release the terrain shader object.
	if (_terrainShader)
	{
		_terrainShader->Destroy();
		delete _terrainShader;
		_terrainShader = 0;
	}

	// Release the sky dome shader object.
	if (_skydomeShader)
	{
		_skydomeShader->Destroy();
		delete _skydomeShader;
		_skydomeShader = 0;
	}

	// Release the Colour shader object.
	if(_colourShader)
	{
		_colourShader->Destroy();
		delete _colourShader;
		_colourShader = 0;
	}

	// Release the TargaTexture shader object.
	if (_textureShader)
	{
		_textureShader->Destroy();
		delete _textureShader;
		_textureShader = 0;
	}

	// Release the light shader object.
	if (_lightShader)
	{
		_lightShader->Destroy();
		delete _lightShader;
		_lightShader = 0;
	}

	return;
}

bool ShaderManager::RenderColourShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
										   XMMATRIX projectionMatrix)
{
	return _colourShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
}

bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return _textureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection,
	XMFLOAT4 diffuseColor)
{
	return _lightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
}

bool ShaderManager::RenderSkyDomeShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, XMFLOAT4 apexColor, XMFLOAT4 centerColor)
{
	return _skydomeShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor);
}

bool ShaderManager::RenderTerrainShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* normalMap,
	ID3D11ShaderResourceView* normalMap2, ID3D11ShaderResourceView* normalMap3,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	return _terrainShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, normalMap, normalMap2, normalMap3,
		lightDirection, diffuseColor);
}

bool ShaderManager::RenderDeferredShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return _deferredShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManager::RenderDeferredLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colourTexture, ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection)
{
	return _deferredLightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colourTexture, normalTexture, lightDirection);
}

bool ShaderManager::RenderDepthShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix)
{
	return _depthShader->Render(deviceContext, indexCount, worldMatrix, lightViewMatrix, lightProjectionMatrix);
}

bool ShaderManager::RenderShadowShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, ID3D11ShaderResourceView * texture, ID3D11ShaderResourceView * depthMapTexture, 
	XMFLOAT3 lightPosition, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor)
{
	return _shadowShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture, depthMapTexture, lightPosition, ambientColor, diffuseColor);
}
