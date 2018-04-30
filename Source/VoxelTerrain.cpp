#include "VoxelTerrain.h"

VoxelTerrain::VoxelTerrain()
{
}

bool VoxelTerrain::Initialize(ID3D11Device * device, char * modelFilename, int textureIndex)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	_chunk = new VoxelChunk;
	result = _chunk->Initialize(device, modelFilename, textureIndex, _model, _vertexCount, _indexCount);
	if (!result)
	{
		return false;
	}

	_chunkCount = 1;

	return true;
}

void VoxelTerrain::Render(ID3D11DeviceContext * deviceContext)
{
	_chunk->Render(deviceContext);
}

int VoxelTerrain::GetIndexCount()
{
	return _chunk->GetIndexCount();
}

bool VoxelTerrain::LoadModel(char * filename)
{
	std::ifstream fin;
	char input;
	int i;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> _vertexCount;

	// Set the number of indices to be the same as the vertex count.
	_indexCount = _vertexCount;

	// Create the model using the vertex count that was read in.
	_model = new VoxelChunk::ModelType[_vertexCount];
	if (!_model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i<_vertexCount; i++)
	{
		fin >> _model[i].x >> _model[i].y >> _model[i].z;
		fin >> _model[i].tu >> _model[i].tv;
		fin >> _model[i].nx >> _model[i].ny >> _model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}
