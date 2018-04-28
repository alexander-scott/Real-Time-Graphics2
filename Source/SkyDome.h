#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class SkyDome
{
private:
	struct ModelType
	{
		float X, Y, Z;
		float Tu, Tv;
		float Nx, Ny, Nz;
	};

	struct VertexType
	{
		XMFLOAT3 Position;
	};

public:
	SkyDome();
	~SkyDome();

	bool Initialize(ID3D11Device* device);
	void Destroy();
	void Draw(ID3D11DeviceContext* context);

	int GetIndexCount();
	XMFLOAT4 GetApexColor();
	XMFLOAT4 GetCenterColor();

private:
	bool LoadSkyDomeModel(char* filePath);
	void ReleaseSkyDomeModel();

	bool InitializeBuffers(ID3D11Device* device);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext* context);

private:
	ModelType *			_model;
	int					_vertexCount, _indexCount;
	ID3D11Buffer*		_vertexBuffer, *_indexBuffer;
	XMFLOAT4			_apexColour, _centerColour;
};