#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include <stdio.h>
#include <vector>

#include "TerrainCell.h"
#include "Frustum.h"

using namespace DirectX;
using namespace std;

class ProceduralTerrain
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
		float Tu2, Tv2;
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
	ProceduralTerrain();
	ProceduralTerrain(const ProceduralTerrain&);
	~ProceduralTerrain();

	bool Initialize(ID3D11Device*, char*);
	void Destroy();

	void Update();

	bool RenderCell(ID3D11DeviceContext*, int, Frustum*);
	void RenderCellLines(ID3D11DeviceContext*, int);

	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();

	int GetRenderCount();
	int GetCellsDrawn();
	int GetCellsCulled();

	bool GetHeightAtPosition(float, float, float&);

private:
	bool LoadSetupFile(char*);

	bool ProcGenHeightMap();
	void DiamondSquareAlgorithm(float cornerHeight, float randomRange, float heightScalar);
	void FaultLineAlgorithm();

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

	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

	float RandomRange(float min, float max);
	float GetSquareAverage(std::vector< float > &vector, int i, int j, int step, float randomRange, float smoothingValue);
	float Fit(float x);

private:
	int					_vertexCount, _indexCount;

	int					_terrainHeight, _terrainWidth;
	float				_heightScale;
	char*				_terrainFilename, *_colourMapFilename;
	HeightMapType*		_heightMap;
	ModelType*			_terrainModel;
	TerrainCell*		_terrainCells;
	int					_cellCount, _renderCount, _cellsDrawn, _cellsCulled;
};