#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class IShader
{
protected:
	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

public:
	IShader();
	IShader(const IShader&);
	~IShader();

	bool Initialize(ID3D11Device* device, HWND hwnd, WCHAR* pixelShaderPath, WCHAR* vertexShaderPath);
	void Destroy();

protected:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) = 0;
	virtual void DestroyShader() = 0;
	virtual void RenderShader(ID3D11DeviceContext*, int) = 0;

	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

protected:
	ID3D11VertexShader *	_vertexShader;
	ID3D11PixelShader*		_pixelShader;
	ID3D11InputLayout*		_layout;
	ID3D11Buffer*			_matrixBuffer;
};