#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "IShader.h"

using namespace DirectX;
using namespace std;

class SkyDomeShader : public IShader
{
private:
	struct ColorBufferType
	{
		XMFLOAT4 ApexColour;
		XMFLOAT4 CentreColour;
	};

public:
	SkyDomeShader();
	SkyDomeShader(const SkyDomeShader&);
	~SkyDomeShader();

	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) override;
	void DestroyShader() override;
	void RenderShader(ID3D11DeviceContext*, int) override;

private:
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4);

	ID3D11Buffer*	_colourBuffer;
};