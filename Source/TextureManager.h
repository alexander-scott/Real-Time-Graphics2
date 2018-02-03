#pragma once

#include "Texture.h"

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
	Texture* m_TextureArray;
	int m_textureCount;
};