#pragma once

const int BUFFER_COUNT = 2;

#include <d3d11.h>
#include <DirectXMath.h>

class RenderTextureBuffer
{
public:
	RenderTextureBuffer();
	~RenderTextureBuffer();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTargets(ID3D11DeviceContext*);
	void ClearRenderTargets(ID3D11DeviceContext*, float, float, float, float);

	ID3D11ShaderResourceView* GetShaderResourceView(int);

private:
	int							_textureWidth, _textureHeight;
	ID3D11Texture2D*			_renderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView*		_renderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView*	_shaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D*			_depthStencilBuffer;
	ID3D11DepthStencilView*		_depthStencilView;
	D3D11_VIEWPORT				_viewport;
};