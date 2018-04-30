#include "VoxelChunk.h"

VoxelChunk::VoxelChunk()
{
	// Create the blocks
	m_pBlocks = new Voxel**[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		m_pBlocks[i] = new Voxel*[CHUNK_SIZE];

		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			m_pBlocks[i][j] = new Voxel[CHUNK_SIZE];
			for (int k = 0; k < CHUNK_SIZE; k++)
			{
				if (sqrt((float)(i - CHUNK_SIZE / 2)*(i - CHUNK_SIZE / 2) + (j - CHUNK_SIZE / 2)*(j - CHUNK_SIZE / 2) + (k - CHUNK_SIZE / 2)*(k - CHUNK_SIZE / 2)) <= CHUNK_SIZE / 2)
				{
					m_pBlocks[i][j][k].SetActive(true);
				}
				else
				{
					m_pBlocks[i][j][k].SetActive(false);
				}
			}
		}
	}
}

VoxelChunk::~VoxelChunk()
{
	// Delete the blocks
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			delete[] m_pBlocks[i][j];
		}

		delete[] m_pBlocks[i];
	}
	delete[] m_pBlocks;
}

bool VoxelChunk::Initialize(ID3D11Device * device, char * modelFilename, int textureIndex, ModelType* model, int vertexCount, int indexCount, int xPos, int yPos, int zPos)
{
	bool result;

	_model = model;

	_xPos = xPos;
	_yPos = yPos;
	_zPos = zPos;

	CreateMesh();

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device, vertexCount, indexCount);
	if (!result)
	{
		return false;
	}

	return true;
}

int VoxelChunk::GetIndexCount()
{
	return _indexCount;
}

void VoxelChunk::Update(float deltaTime)
{
}

void VoxelChunk::Render(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void VoxelChunk::CreateMesh()
{
	_newVoxels.clear();

	bool lDefault = false;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (m_pBlocks[x][y][z].IsActive() == false)
				{
					// Don't create triangle data for inactive blocks
					continue;
				}

				bool lXNegative = lDefault;
				if (x > 0)
					lXNegative = m_pBlocks[x - 1][y][z].IsActive();

				bool lXPositive = lDefault;
				if (x < CHUNK_SIZE - 1)
					lXPositive = m_pBlocks[x + 1][y][z].IsActive();

				bool lYNegative = lDefault;
				if (y > 0)
					lYNegative = m_pBlocks[x][y - 1][z].IsActive();

				bool lYPositive = lDefault;
				if (y < CHUNK_SIZE - 1)
					lYPositive = m_pBlocks[x][y + 1][z].IsActive();

				bool lZNegative = lDefault;
				if (z > 0)
					lZNegative = m_pBlocks[x][y][z - 1].IsActive();

				bool lZPositive = lDefault;
				if (z < CHUNK_SIZE - 1)
					lZPositive = m_pBlocks[x][y][z + 1].IsActive();

				if (!lXNegative || !lXPositive || !lYNegative || !lYPositive || !lZNegative || !lZPositive)
				{
					continue;
				}

				NewVoxel newVox;
				newVox.X = x;
				newVox.Y = y;
				newVox.Z = z;
				newVox.Index = (x * CHUNK_SIZE) + (y * CHUNK_SIZE) + z;

				_newVoxels.push_back(newVox);
			}
		}
	}

	if (_newVoxels.size() == 0)
	{
		_hasBlocks = false;
	}
	else
	{
		_hasBlocks = true;
	}
}

bool VoxelChunk::InitializeBuffers(ID3D11Device * device, int vertexCount, int indexCount)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	int voxelCount = _newVoxels.size();

	_vertexCount = vertexCount * voxelCount;
	_indexCount = indexCount * voxelCount;

	// Create the vertex array.
	vertices = new VertexType[_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[_indexCount];
	if (!indices)
	{
		return false;
	}

	for (int j = 0; j < voxelCount; j++)
	{
		// Load the vertex array and index array with data.
		for (i = 0; i<vertexCount; i++)
		{
			int index = (j * vertexCount) + i;
			
			vertices[index].position = XMFLOAT3(_model[i].x * _newVoxels[j].X * _xPos, _model[i].y * _newVoxels[j].Y * _yPos, _model[i].z * _newVoxels[j].Z * _zPos);
			vertices[index].texture = XMFLOAT2(_model[i].tu, _model[i].tv);
			vertices[index].normal = XMFLOAT3(_model[i].nx, _model[i].ny, _model[i].nz);

			indices[index] = index;
		}
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}
