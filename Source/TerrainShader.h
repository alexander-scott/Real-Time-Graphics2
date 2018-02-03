#pragma once

#include "IShader.h"

using namespace DirectX;
using namespace std;

class TerrainShader : public IShader
{
public:
	struct LightBufferType
	{
		XMFLOAT4	DiffuseColor;
		XMFLOAT3	LightDirection;
		float		Padding;
	};

	TerrainShader();
	TerrainShader(const TerrainShader&);
	~TerrainShader();

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void DestroyShader() override;
	void RenderShader(ID3D11DeviceContext*, int) override;

private:
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

	ID3D11SamplerState*		_sampleState;
	ID3D11Buffer*			_lightBuffer;
};