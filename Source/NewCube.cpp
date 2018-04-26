#include "NewCube.h"

NewCube::NewCube()
{
	_model = nullptr;
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
}

NewCube::NewCube(const NewCube &)
{
}

NewCube::~NewCube()
{
}

bool NewCube::Initialize(ID3D11Device * device)
{
	bool result;

	// Load in the sky dome model.
	result = LoadSkyDomeModel("Source/Skydome/cube.txt");
	if (!result)
	{
		return false;
	}

	// Load the sky dome into a vertex and index buffer for rendering.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Set the Colour at the top of the sky dome.
	_apexColour = XMFLOAT4(0.0f, 0.05f, 0.6f, 1.0f);

	// Set the Colour at the center of the sky dome.
	_centerColour = XMFLOAT4(0.0f, 0.5f, 0.8f, 1.0f);

	_transform = new Transform;

	return true;
}

void NewCube::Destroy()
{
	// Release the vertex and index buffer that were used for rendering the sky dome.
	ReleaseBuffers();

	// Release the sky dome model.
	ReleaseSkyDomeModel();

	return;
}

void NewCube::Draw(ID3D11DeviceContext * deviceContext)
{
	// Render the sky dome.
	RenderBuffers(deviceContext);

	return;
}

int NewCube::GetIndexCount()
{
	return _indexCount;
}

XMFLOAT4 NewCube::GetApexColor()
{
	return _apexColour;
}

XMFLOAT4 NewCube::GetCenterColor()
{
	return _centerColour;
}

bool NewCube::LoadSkyDomeModel(char * filename)
{
	ifstream fin;
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
	_model = new ModelType[_vertexCount];
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
		fin >> _model[i].X >> _model[i].Y >> _model[i].Z;
		fin >> _model[i].Tu >> _model[i].Tv;
		fin >> _model[i].Nx >> _model[i].Ny >> _model[i].Nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void NewCube::ReleaseSkyDomeModel()
{
	if (_model)
	{
		delete[] _model;
		_model = 0;
	}

	return;
}

bool NewCube::InitializeBuffers(ID3D11Device * device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

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

	// Load the vertex array and index array with data.
	for (i = 0; i<_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(_model[i].X, _model[i].Y, _model[i].Z);
		vertices[i].texture = XMFLOAT2(_model[i].Tu, _model[i].Tv);
		vertices[i].normal = XMFLOAT3(_model[i].Nx, _model[i].Ny, _model[i].Nz);
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
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

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
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

void NewCube::ReleaseBuffers()
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

void NewCube::RenderBuffers(ID3D11DeviceContext * deviceContext)
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
