#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>

#include "Transform.h"

using namespace DirectX;
using namespace std;

class Cube
{
private:
	struct ModelType
	{
		float X, Y, Z;
		float Tu, Tv;
		float Nx, Ny, Nz;
	};

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

	Transform* GetTransform() { return _transform; }

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext* context);

	bool LoadSkyDomeModel(char* filename);

	int					_vertexCount, _indexCount;
	ID3D11Buffer*		_vertexBuffer, *_indexBuffer;

	ModelType *			_model;

	Transform*		_transform;
};

