#pragma once

#include "VoxelChunk.h"
#include <fstream>

const int CHUNK_COUNT = 2;

class VoxelTerrain
{
public:
	VoxelTerrain();

	bool Initialize(ID3D11Device* device, char* modelFilename, int textureIndex);

	void Render(ID3D11DeviceContext* deviceContext, int x, int y, int z);

	int GetIndexCount(int x, int y, int z);
	bool HasBlocks(int x, int y, int z);

private:
	bool LoadModel(char* filename);

	int						_vertexCount, _indexCount;
	int						_chunkCount;
	VoxelChunk***			_chunk;
	VoxelChunk::ModelType*	_model;
};