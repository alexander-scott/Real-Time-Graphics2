#pragma once

#include "VoxelChunk.h"
#include <fstream>

class VoxelTerrain
{
public:
	VoxelTerrain();

	bool Initialize(ID3D11Device* device, char* modelFilename, int textureIndex);

	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();

private:
	bool LoadModel(char* filename);

	int						_vertexCount, _indexCount;
	int						_chunkCount;
	VoxelChunk*				_chunk;
	VoxelChunk::ModelType*	_model;
};