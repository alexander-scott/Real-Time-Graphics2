#pragma once

#include "Voxel.h"

#include <d3d11.h>
#include <directxmath.h>

#include <vector>

using namespace DirectX;

class VoxelChunk
{
private:
	struct NewVoxel 
	{
		public:
			int X;
			int Y;
			int Z;
			int Index;
	};
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
public:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	VoxelChunk();
	~VoxelChunk();

	bool Initialize(ID3D11Device* device, char* modelFilename, int textureIndex, ModelType* model, int vertexCount, int indexCount, int xPos, int yPos, int zPos);

	int GetIndexCount();

	void Update(float deltaTime);

	void Render(ID3D11DeviceContext* deviceContext);

	void CreateMesh();

	bool HasBlocks() { return _hasBlocks; }

	static const int CHUNK_SIZE = 32;

private:
	bool InitializeBuffers(ID3D11Device* device, int vertexCount, int indexCount);

	// The blocks data
	Voxel*** m_pBlocks;

	ModelType*			_model;

	ID3D11Buffer		*_vertexBuffer, *_indexBuffer;
	int					_vertexCount, _indexCount;

	bool				_hasBlocks;
	std::vector<NewVoxel> _newVoxels;
	int						_xPos, _yPos, _zPos;
};
