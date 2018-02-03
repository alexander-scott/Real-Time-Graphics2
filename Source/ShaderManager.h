#ifndef _SHADERMANAGERCLASS_H_
#define _SHADERMANAGERCLASS_H_

#include "DX11Instance.h"
#include "ColourShader.h"
#include "TextureShader.h"

class ShaderManager
{
public:
	ShaderManager();
	ShaderManager(const ShaderManager&);
	~ShaderManager();

	bool Initialize(ID3D11Device*, HWND);
	void Destroy();

	bool RenderColourShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);
	bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
	ColourShader*		_colourShader;
	TextureShader*		_textureShader;
};

#endif