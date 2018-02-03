#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	_colourShader = nullptr;
	_textureShader = nullptr;
	_lightShader = nullptr;
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

	// Create the color shader object.
	_colourShader = new ColourShader;
	if(!_colourShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = _colourShader->Initialize(device, hwnd, L"Source/Shaders/color.ps", L"Source/Shaders/color.vs");
	if(!result)
	{
		return false;
	}

	// Create the Texture shader object.
	_textureShader = new TextureShader;
	if (!_textureShader)
	{
		return false;
	}

	// Initialize the Texture shader object.
	result = _textureShader->Initialize(device, hwnd, L"Source/Shaders/texture.ps", L"Source/Shaders/texture.vs");
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
	result = _lightShader->Initialize(device, hwnd, L"Source/Shaders/light.ps", L"Source/Shaders/light.vs");
	if (!result)
	{
		return false;
	}

	return true;
}

void ShaderManager::Destroy()
{
	// Release the color shader object.
	if(_colourShader)
	{
		_colourShader->Destroy();
		delete _colourShader;
		_colourShader = 0;
	}

	// Release the Texture shader object.
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
