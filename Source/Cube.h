#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class Cube
{
private:
	struct VertexType
	{
		XMFLOAT3 PosL;
		XMFLOAT3 NormL;
		XMFLOAT2 Tex;
	};

public:
	Cube();
	~Cube();

	bool Initialize(ID3D11Device* device);
	void Destroy();
	void Draw(ID3D11DeviceContext* context);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext* context);

	int					_vertexCount, _indexCount;
	ID3D11Buffer*		_vertexBuffer, *_indexBuffer;
};

