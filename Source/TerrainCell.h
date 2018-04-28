#pragma once

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class TerrainCell
{
private:
	struct ModelType
	{
		float X, Y, Z;
		float Tu, Tv;
		float Nx, Ny, Nz;
		float Tx, Ty, Tz;
		float Bx, By, Bz;
		float R, G, B;
		float Tu2, Tv2;
	};

	struct VertexType
	{
		XMFLOAT3 Position;
		XMFLOAT2 Texture;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;
		XMFLOAT3 Binormal;
		XMFLOAT3 Colour;
		XMFLOAT2 Texture2;
	};

	struct VectorType
	{
		float X, Y, Z;
	};

	struct ColorVertexType
	{
		XMFLOAT3 Position;
		XMFLOAT4 Colour;
	};

public:
	TerrainCell();
	~TerrainCell();

	bool Initialize(ID3D11Device* device, void* terrainModelPtr, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth);
	void Destroy();
	void Draw(ID3D11DeviceContext* deviceContext);
	void DrawLineBuffers(ID3D11DeviceContext* deviceContext);

	int GetVertexCount();
	int GetIndexCount();
	int GetLineBuffersIndexCount();
	void GetCellDimensions(float& maxWidth, float& maxHeight, float& maxDepth, float& minWidth, float& minHeight, float& minDepth);

private:
	bool InitializeBuffers(ID3D11Device* device, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth,	int terrainWidth, ModelType* terrainModel);
	void DestroyBuffers();
	void DrawBuffers(ID3D11DeviceContext* deviceContext);
	void CalculateCellDimensions();
	bool BuildLineBuffers(ID3D11Device* deviceContext);
	void DestroyLineBuffers();

public:
	VectorType *		VertexList;

private:
	int					_vertexCount, _indexCount, _lineIndexCount;
	ID3D11Buffer		*_vertexBuffer, *_indexBuffer, *_lineVertexBuffer, *_lineIndexBuffer;
	float				_maxWidth, _maxHeight, _maxDepth, _minWidth, _minHeight, _minDepth;
	float				_positionX, _positionY, _positionZ;
};