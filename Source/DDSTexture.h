#pragma once

#include "ITexture.h"

#include <iostream>
#include <fstream>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "DDSTextureLoader.h"

class DDSTexture : public ITexture
{
public:
	DDSTexture();
	~DDSTexture();

	void SetPath(std::wstring path) { _path = path; }

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;
	void Destroy() override;

private:
	std::wstring _path;
};

