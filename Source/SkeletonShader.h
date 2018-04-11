#pragma once

#include "IShader.h"

class SkeletonShader : public IShader
{
public:
	struct LightBufferType
	{
		XMFLOAT4	DiffuseColour;
		XMFLOAT3	LightDirection;
		float		Padding;
	};

	SkeletonShader();
	~SkeletonShader();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1,
		ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, ID3D11ShaderResourceView* texture4,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void DestroyShader() override;
	void RenderShader(ID3D11DeviceContext*, int) override;

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1,
		ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, ID3D11ShaderResourceView* texture4,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor);

	ID3D11SamplerState*		_sampleState;
	ID3D11Buffer*			_lightBuffer;
};

