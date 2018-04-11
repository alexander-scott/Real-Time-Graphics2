#pragma once

#include <d3d11.h>
#include <stdio.h>

class ITexture
{
public:
	virtual bool Initialize(ID3D11Device*, ID3D11DeviceContext*) = 0;
	virtual void Destroy() = 0;

	ID3D11ShaderResourceView * GetTexture() { return _textureView; }

protected:
	ID3D11ShaderResourceView * _textureView;
};