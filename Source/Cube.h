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
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:
	Cube();
	Cube(const Cube&);
	~Cube();

	bool Initialize(ID3D11Device*);
	void Destroy();
	void Draw(ID3D11DeviceContext*);

	int GetIndexCount();
	XMFLOAT4 GetApexColor();
	XMFLOAT4 GetCenterColor();

	Transform* GetTransform() { return _transform; }

private:
	bool LoadSkyDomeModel(char*);
	void ReleaseSkyDomeModel();

	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ModelType *			_model;
	int					_vertexCount, _indexCount;
	ID3D11Buffer*		_vertexBuffer, *_indexBuffer;
	XMFLOAT4			_apexColour, _centerColour;

	Transform*		_transform;
};