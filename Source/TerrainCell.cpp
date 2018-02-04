#include "TerrainCell.h"

TerrainCell::TerrainCell()
{
	_vertexList = nullptr;
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
	_lineVertexBuffer = nullptr;
	_lineIndexBuffer = nullptr;
}

TerrainCell::TerrainCell(const TerrainCell &)
{
}

TerrainCell::~TerrainCell()
{
}

bool TerrainCell::Initialize(ID3D11Device* device, void* terrainModelPtr, int nodeIndexX, int nodeIndexY,
	int cellHeight, int cellWidth, int terrainWidth)
{
	ModelType* terrainModel;
	bool result;

	// Coerce the pointer to the terrain model into the model type.
	terrainModel = (ModelType*)terrainModelPtr;

	// Load the rendering buffers with the terrain data for this cell index.
	result = InitializeBuffers(device, nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, terrainModel);
	if (!result)
	{
		return false;
	}

	// Release the pointer to the terrain model now that we no longer need it.
	terrainModel = 0;

	// Calculuate the dimensions of this cell.
	CalculateCellDimensions();

	// Build the debug line buffers to produce the bounding box around this cell.
	result = BuildLineBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void TerrainCell::Destroy()
{
	// Release the line rendering buffers.
	DestroyLineBuffers();

	// Release the cell rendering buffers.
	DestroyBuffers();

	return;
}

void TerrainCell::Draw(ID3D11DeviceContext * deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	DrawBuffers(deviceContext);

	return;
}

void TerrainCell::DrawLineBuffers(ID3D11DeviceContext * deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(ColorVertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &_lineVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(_lineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}

int TerrainCell::GetVertexCount()
{
	return _vertexCount;
}

int TerrainCell::GetIndexCount()
{
	return _indexCount;
}

int TerrainCell::GetLineBuffersIndexCount()
{
	return _lineIndexCount;
}

void TerrainCell::GetCellDimensions(float& maxWidth, float& maxHeight, float& maxDepth,
	float& minWidth, float& minHeight, float& minDepth)
{
	maxWidth = _maxWidth;
	maxHeight = _maxHeight;
	maxDepth = _maxDepth;
	minWidth = _minWidth;
	minHeight = _minHeight;
	minDepth = _minDepth;
	return;
}

bool TerrainCell::InitializeBuffers(ID3D11Device* device, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth,
	int terrainWidth, ModelType* terrainModel)
{
	VertexType* vertices;
	unsigned long* indices;
	int i, j, modelIndex, index;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Calculate the number of vertices in this terrain cell.
	_vertexCount = (cellHeight - 1) * (cellWidth - 1) * 6;

	// Set the index count to the same as the vertex count.
	_indexCount = _vertexCount;

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

	// Setup the indexes into the terrain model data and the local vertex/index array.
	modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	index = 0;

	// Load the vertex array and index array with data.
	for (j = 0; j<(cellHeight - 1); j++)
	{
		for (i = 0; i<((cellWidth - 1) * 6); i++)
		{
			vertices[index].Position = XMFLOAT3(terrainModel[modelIndex].X, terrainModel[modelIndex].Y, terrainModel[modelIndex].Z);
			vertices[index].Texture = XMFLOAT2(terrainModel[modelIndex].Tu, terrainModel[modelIndex].Tv);
			vertices[index].Normal = XMFLOAT3(terrainModel[modelIndex].Nx, terrainModel[modelIndex].Ny, terrainModel[modelIndex].Nz);
			vertices[index].Tangent = XMFLOAT3(terrainModel[modelIndex].Tx, terrainModel[modelIndex].Ty, terrainModel[modelIndex].Tz);
			vertices[index].Binormal = XMFLOAT3(terrainModel[modelIndex].Bx, terrainModel[modelIndex].By, terrainModel[modelIndex].Bz);
			vertices[index].Colour = XMFLOAT3(terrainModel[modelIndex].R, terrainModel[modelIndex].G, terrainModel[modelIndex].B);
			indices[index] = index;
			modelIndex++;
			index++;
		}
		modelIndex += (terrainWidth * 6) - (cellWidth * 6);
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

	// Create a public vertex array that will be used for accessing vertex information about this cell.
	_vertexList = new VectorType[_vertexCount];
	if (!_vertexList)
	{
		return false;
	}

	// Keep a local copy of the vertex Position data for this cell.
	for (i = 0; i<_vertexCount; i++)
	{
		_vertexList[i].X = vertices[i].Position.x;
		_vertexList[i].Y = vertices[i].Position.y;
		_vertexList[i].Z = vertices[i].Position.z;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void TerrainCell::DestroyBuffers()
{
	// Release the public vertex list.
	if (_vertexList)
	{
		delete[] _vertexList;
		_vertexList = 0;
	}

	// Release the index buffer.
	if (_indexBuffer)
	{
		_indexBuffer->Release();
		_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (_vertexBuffer)
	{
		_vertexBuffer->Release();
		_vertexBuffer = 0;
	}

	return;
}

void TerrainCell::DrawBuffers(ID3D11DeviceContext * deviceContext)
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

void TerrainCell::CalculateCellDimensions()
{
	int i;
	float width, height, depth;

	// Initialize the dimensions of the node.
	_maxWidth = -1000000.0f;
	_maxHeight = -1000000.0f;
	_maxDepth = -1000000.0f;

	_minWidth = 1000000.0f;
	_minHeight = 1000000.0f;
	_minDepth = 1000000.0f;

	for (i = 0; i<_vertexCount; i++)
	{
		width = _vertexList[i].X;
		height = _vertexList[i].Y;
		depth = _vertexList[i].Z;

		// Check if the width exceeds the minimum or maximum.
		if (width > _maxWidth)
		{
			_maxWidth = width;
		}
		if (width < _minWidth)
		{
			_minWidth = width;
		}

		// Check if the height exceeds the minimum or maximum.
		if (height > _maxHeight)
		{
			_maxHeight = height;
		}
		if (height < _minHeight)
		{
			_minHeight = height;
		}

		// Check if the depth exceeds the minimum or maximum.
		if (depth > _maxDepth)
		{
			_maxDepth = depth;
		}
		if (depth < _minDepth)
		{
			_minDepth = depth;
		}
	}

	// Calculate the center Position of this cell.
	_positionX = (_maxWidth - _minWidth) + _minWidth;
	_positionY = (_maxHeight - _minHeight) + _minHeight;
	_positionZ = (_maxDepth - _minDepth) + _minDepth;

	return;
}

bool TerrainCell::BuildLineBuffers(ID3D11Device * device)
{
	ColorVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	XMFLOAT4 lineColor;
	int index, vertexCount, indexCount;


	// Set the Colour of the lines to orange.
	lineColor = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);

	// Set the number of vertices in the vertex array.
	vertexCount = 24;

	// Set the number of indices in the index array.
	indexCount = vertexCount;

	// Create the vertex array.
	vertices = new ColorVertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ColorVertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Load the vertex and index array with data.
	index = 0;

	// 8 Horizontal lines.
	vertices[index].Position = XMFLOAT3(_minWidth, _minHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _minHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _minHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _minHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _minHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _minHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _minHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _minHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _maxHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _maxHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _maxHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _maxHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _maxHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _maxHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _maxHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _maxHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	// 4 Verticle lines.
	vertices[index].Position = XMFLOAT3(_maxWidth, _maxHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _minHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _maxHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _minHeight, _maxDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _maxHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_maxWidth, _minHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _maxHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;
	index++;

	vertices[index].Position = XMFLOAT3(_minWidth, _minHeight, _minDepth);
	vertices[index].Colour = lineColor;
	indices[index] = index;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_lineVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_lineIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Store the index count for rendering.
	_lineIndexCount = indexCount;

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void TerrainCell::DestroyLineBuffers()
{
	// Release the index buffer.
	if (_lineIndexBuffer)
	{
		_lineIndexBuffer->Release();
		_lineIndexBuffer = 0;
	}

	// Release the vertex buffer.
	if (_lineVertexBuffer)
	{
		_lineVertexBuffer->Release();
		_lineVertexBuffer = 0;
	}

	return;
}
