#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>

#include "TerrainCell.h"

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
		XMFLOAT3 Tangent;
		XMFLOAT3 Binormal;
		XMFLOAT3 Colour;
	};

	struct HeightMapType
	{
		float X, Y, Z;
		float Nx, Ny, Nz;
		float R, G, B;
	};

	struct ModelType
	{
		float X, Y, Z;
		float Tu, Tv;
		float Nx, Ny, Nz;
		float Tx, Ty, Tz;
		float Bx, By, Bz;
		float R, G, B;
	};

	struct VectorType
	{
		float X, Y, Z;
	};

	struct TempVertexType
	{
		float X, Y, Z;
		float Tu, Tv;
		float Nx, Ny, Nz;
	};

public:
	Terrain();
	Terrain(const Terrain&);
	~Terrain();

	bool Initialize(ID3D11Device*, char*);
	void Destroy();

	bool RenderCell(ID3D11DeviceContext*, int);
	void RenderCellLines(ID3D11DeviceContext*, int);

	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();

	int GetIndexCount();

private:
	bool LoadSetupFile(char*);
	bool LoadBitmapHeightMap();
	bool LoadRawHeightMap();
	void DestroyHeightMap();
	void SetTerrainCoordinates();
	bool CalculateNormals();
	bool LoadColourMap();
	bool BuildTerrainModel();
	void DestroyTerrainModel();

	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

	bool LoadTerrainCells(ID3D11Device*);
	void DestroyTerrainCells();

private:
	int					_vertexCount, _indexCount;

	int					_terrainHeight, _terrainWidth;
	float				_heightScale;
	char*				_terrainFilename, *_colourMapFilename;
	HeightMapType*		_heightMap;
	ModelType*			_terrainModel;
	TerrainCell*		_terrainCells;
	int					_cellCount;
};

#endif