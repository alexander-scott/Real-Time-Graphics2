#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "IShader.h"

using namespace DirectX;
using namespace std;

struct SurfaceInfo
{
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 SpecularMtrl;
};

struct LightStruct
{
	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;

	float SpecularPower;
	XMFLOAT3 LightVecW;
};

struct LightBufferType
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	SurfaceInfo surface;

	LightStruct light;

	XMFLOAT3 EyePosW;
	float HasTexture;
};

class CubeShader : public IShader
{
public:
	CubeShader();
	~CubeShader();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
		SurfaceInfo surface, LightStruct light, XMFLOAT3 eyePosW, float hasTexture, ID3D11ShaderResourceView* texture);

protected:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename) override;
	void DestroyShader() override;
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) override;

private:
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
		SurfaceInfo surface, LightStruct light, XMFLOAT3 eyePosW, float hasTexture, ID3D11ShaderResourceView* texture);

	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	ID3D11SamplerState * _samplerLinear;
};

