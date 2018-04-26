#include "Cube.h"

Cube::Cube()
{
}


Cube::~Cube()
{
}

bool Cube::Initialize(ID3D11Device * device)
{
	bool result;

	// Load the cube into a vertex and index buffer for rendering.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void Cube::Destroy()
{
	// Release the vertex and index buffer that were used for rendering the cube.
	ReleaseBuffers();

	return;
}

void Cube::Draw(ID3D11DeviceContext * context)
{
	// Render the cube.
	RenderBuffers(context);

	return;
}

int Cube::GetIndexCount()
{
	return _indexCount;
}

bool Cube::InitializeBuffers(ID3D11Device * device)
{
	bool result;

	// Create vertex buffer
	VertexType vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	_vertexCount = 24;

	D3D11_BUFFER_DESC bufferDescVerticies;
	ZeroMemory(&bufferDescVerticies, sizeof(bufferDescVerticies));
	bufferDescVerticies.Usage = D3D11_USAGE_DEFAULT;
	bufferDescVerticies.ByteWidth = sizeof(VertexType) * 24;
	bufferDescVerticies.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescVerticies.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataVerticies;
	ZeroMemory(&InitDataVerticies, sizeof(InitDataVerticies));
	InitDataVerticies.pSysMem = vertices;

	result = device->CreateBuffer(&bufferDescVerticies, &InitDataVerticies, &_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create index buffer
	WORD indices[] =
	{
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
	};

	_indexCount = 36;

	D3D11_BUFFER_DESC bufferDescIndicies;
	ZeroMemory(&bufferDescIndicies, sizeof(bufferDescIndicies));

	bufferDescIndicies.Usage = D3D11_USAGE_DEFAULT;
	bufferDescIndicies.ByteWidth = sizeof(WORD) * 36;
	bufferDescIndicies.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDescIndicies.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataIndicies;
	ZeroMemory(&InitDataIndicies, sizeof(InitDataIndicies));
	InitDataIndicies.pSysMem = indices;
	result = device->CreateBuffer(&bufferDescIndicies, &InitDataIndicies, &_indexBuffer);

	if (FAILED(result))
	{
		return false;
	}
		
	return true;
}

void Cube::ReleaseBuffers()
{
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

void Cube::RenderBuffers(ID3D11DeviceContext * context)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
