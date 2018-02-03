#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>

using namespace DirectX;
using namespace std;

class Terrain
{
private:
	struct VertexType
	{
		XMFLOAT3 Position;
		XMFLOAT2 Texture;
		XMFLOAT3 Normal;
		XMFLOAT3 Colour;
	};

	struct HeightMapType
	{
		float X, Y, Z;
		float NX, NY, NZ;
		float R, G, B;
	};

	struct ModelType
	{
		float X, Y, Z;
		float Tu, Tv;
		float NX, NY, NZ;
		float R, G, B;
	};

	struct VectorType
	{
		float X, Y, Z;
	};

public:
	Terrain();
	Terrain(const Terrain&);
	~Terrain();

	bool Initialize(ID3D11Device*, char*);
	void Destroy();
	bool Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool LoadSetupFile(char*);
	bool LoadBitmapHeightMap();
	void DestroyHeightMap();
	void SetTerrainCoordinates();
	bool CalculateNormals();
	bool LoadColourMap();
	bool BuildTerrainModel();
	void DestroyTerrainModel();

	bool InitializeBuffers(ID3D11Device*);
	void DestroyBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer		*_vertexBuffer, *_indexBuffer;
	int					_vertexCount, _indexCount;

	int					_terrainHeight, _terrainWidth;
	float				_heightScale;
	char*				_terrainFilename, *_colourMapFilename;
	HeightMapType*		_heightMap;
	ModelType*			_terrainModel;
};

#endif