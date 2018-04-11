#pragma once

#include "ITexture.h"

class JPEGTexture : public ITexture
{
public:
	JPEGTexture();
	~JPEGTexture();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) override;
	void Destroy() override;
};

