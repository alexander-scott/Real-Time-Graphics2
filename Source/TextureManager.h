#pragma once

#include "TargaTexture.h"

class TextureManager
{
public:
	TextureManager(); 
	TextureManager(const TextureManager&);
	~TextureManager();

	bool Initialize(int);
	void Destroy();

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*, int);

	ID3D11ShaderResourceView* GetTexture(int);

private:
	TargaTexture*		_textureArray;
	int				_textureCount;
};