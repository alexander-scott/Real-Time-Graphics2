#include "JPEGTexture.h"

JPEGTexture::JPEGTexture()
{
}

JPEGTexture::~JPEGTexture()
{
}

bool JPEGTexture::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	HRESULT hr = DirectX::CreateDDSTextureFromFile(device, _path.c_str(), NULL, &_textureView);

	if (SUCCEEDED(hr))
	{
		return true;
	}

	return false;
}

void JPEGTexture::Destroy()
{
}
