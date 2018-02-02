#include "ShaderManager.h"

ShaderManager::ShaderManager()
{
	m_ColorShader = 0;
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
	m_ColorShader = new ColourShader;
	if(!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(device, hwnd);
	if(!result)
	{
		return false;
	}

	return true;
}

void ShaderManager::Destroy()
{
	// Release the color shader object.
	if(m_ColorShader)
	{
		m_ColorShader->Destroy();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	return;
}

bool ShaderManager::RenderColorShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
										   XMMATRIX projectionMatrix)
{
	return m_ColorShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
}