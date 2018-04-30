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

	// Initalize the chunks
	_chunk = new VoxelChunk**[CHUNK_COUNT];
	for (int i = 0; i < CHUNK_COUNT; i++)
	{
		_chunk[i] = new VoxelChunk*[CHUNK_COUNT];

		for (int j = 0; j < CHUNK_COUNT; j++)
		{
			_chunk[i][j] = new VoxelChunk[CHUNK_COUNT];
			for (int k = 0; k < CHUNK_COUNT; k++)
			{
				result = _chunk[i][j][k].Initialize(device, modelFilename, textureIndex, _model, _vertexCount, _indexCount, i + 1, j + 1, k + 1);
				if (!result)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void VoxelTerrain::Render(ID3D11DeviceContext * deviceContext, int x, int y, int z)
{
	_chunk[x][y][z].Render(deviceContext);
}

int VoxelTerrain::GetIndexCount(int x, int y, int z)
{
	return _chunk[x][y][z].GetIndexCount();
}

bool VoxelTerrain::HasBlocks(int x, int y, int z)
{
	return _chunk[x][y][z].HasBlocks();
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
