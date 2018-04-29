#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "IShader.h"

using namespace DirectX;
using namespace std;

class DeferredLightShader : public IShader
{
private:
	struct LightBufferType
	{
		XMFLOAT3	LightDirection;
		float		Padding;
	};

public:
	DeferredLightShader();
	~DeferredLightShader();

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void DestroyShader() override;
	void RenderShader(ID3D11DeviceContext*, int) override;

private:
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMFLOAT3);

	ID3D11SamplerState*		_sampleState;
	ID3D11Buffer*			_lightBuffer;
};