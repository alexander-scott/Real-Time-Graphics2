#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	_colourShader = nullptr;
	_textureShader = nullptr;
	_lightShader = nullptr;
	_skydomeShader = nullptr;
	_terrainShader = nullptr;
	_cubeShader = nullptr;
}

ShaderManager::ShaderManager(const ShaderManager& other)
{
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

	// Create the cube shader object.
	_cubeShader = new CubeShader;
	if (!_cubeShader)
	{
		return false;
	}

	// Initialize the cube shader object.
	result = _cubeShader->Initialize(device, hwnd, L"Source/Shaders/CubeShader.fx", L"Source/Shaders/CubeShader.fx");
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

bool ShaderManager::RenderCubeShader(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
	SurfaceInfo surface, LightStruct light, XMFLOAT3 eyePosW, float hasTexture, ID3D11ShaderResourceView * texture)
{
	return _cubeShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, 
		surface, light, eyePosW, hasTexture, texture);
}
