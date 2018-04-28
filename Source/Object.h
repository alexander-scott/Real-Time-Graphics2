#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>

#include "Transform.h"

using namespace std;
using namespace DirectX;

class Object
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	Object();
	~Object();

	bool Initialize(ID3D11Device* device, char* modelFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

	Transform* GetTransform() { return _transform; }

private:
	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadModel(char* filename);
	void ReleaseModel();

private:
	ID3D11Buffer *		_vertexBuffer, *_indexBuffer;
	int					_vertexCount, _indexCount;
	ModelType*			_model;

	Transform*			_transform;
};