#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <stdio.h>

class Texture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Destroy();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadTarga(char*, int&, int&);

private:
	unsigned char*				_targaData;
	ID3D11Texture2D*			_texture;
	ID3D11ShaderResourceView*	_textureView;

};

#endif