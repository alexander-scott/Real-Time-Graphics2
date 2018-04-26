#include "DDSTexture.h"

DDSTexture::DDSTexture()
{
}

DDSTexture::~DDSTexture()
{
}

bool DDSTexture::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	HRESULT hr = DirectX::CreateDDSTextureFromFile(device, _path.c_str(), NULL, &_textureView);

	if (SUCCEEDED(hr))
	{
		return true;
	}

	return false;
}

void DDSTexture::Destroy()
{
	// Release the TargaTexture View resource.
	if (_textureView)
	{
		_textureView->Release();
		_textureView = 0;
	}
}
