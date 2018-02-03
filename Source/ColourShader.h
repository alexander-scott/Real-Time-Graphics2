#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class ColourShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

public:
	ColourShader();
	ColourShader(const ColourShader&);
	~ColourShader();

	bool Initialize(ID3D11Device*, HWND);
	void Destroy();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void DestroyShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader*			_vertexShader;
	ID3D11PixelShader*			_pixelShader;
	ID3D11InputLayout*			_layout;
	ID3D11Buffer*				_matrixBuffer;
};

#endif