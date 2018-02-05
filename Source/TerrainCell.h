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
	TerrainCell(const TerrainCell&);
	~TerrainCell();

	bool Initialize(ID3D11Device*, void*, int, int, int, int, int);
	void Destroy();
	void Draw(ID3D11DeviceContext*);
	void DrawLineBuffers(ID3D11DeviceContext*);

	int GetVertexCount();
	int GetIndexCount();
	int GetLineBuffersIndexCount();
	void GetCellDimensions(float&, float&, float&, float&, float&, float&);

private:
	bool InitializeBuffers(ID3D11Device*, int, int, int, int, int, ModelType*);
	void DestroyBuffers();
	void DrawBuffers(ID3D11DeviceContext*);
	void CalculateCellDimensions();
	bool BuildLineBuffers(ID3D11Device*);
	void DestroyLineBuffers();

public:
	VectorType * _vertexList;

private:
	int					_vertexCount, _indexCount, _lineIndexCount;
	ID3D11Buffer		*_vertexBuffer, *_indexBuffer, *_lineVertexBuffer, *_lineIndexBuffer;
	float				_maxWidth, _maxHeight, _maxDepth, _minWidth, _minHeight, _minDepth;
	float				_positionX, _positionY, _positionZ;
};