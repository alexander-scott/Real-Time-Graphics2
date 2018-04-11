#pragma once

#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>

#include <string>
#include <vector>

#include "TextureManager.h"
#include "Transform.h"

using namespace DirectX;

struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		float tx, float ty, float tz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz),
		tangent(tx, ty, tz) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 biTangent;

	///////////////**************new**************////////////////////
	// Will not be sent to shader
	int StartWeight;
	int WeightCount;
	///////////////**************new**************////////////////////
};

struct Joint
{
	std::wstring name;
	int parentID;

	XMFLOAT3 pos;
	XMFLOAT4 orientation;
};

struct Weight
{
	int jointID;
	float bias;
	XMFLOAT3 pos;
};

struct ModelSubset
{
	int texArrayIndex;
	int numTriangles;

	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;
	std::vector<Weight> weights;

	std::vector<XMFLOAT3> positions;

	ID3D11Buffer* vertBuff;
	ID3D11Buffer* indexBuff;
};

struct Model3D
{
	int numSubsets;
	int numJoints;

	std::vector<Joint> joints;
	std::vector<ModelSubset> subsets;
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	bool Initialize(ID3D11Device * device, ID3D11DeviceContext* context, TextureManager* textureManager, std::wstring fileName);
	void Destroy();

	void Update(float deltaTime);
	void Draw(ID3D11DeviceContext* deviceContext);

	Transform* GetTransform() { return _transform; }

private:
	bool LoadMD5Model(ID3D11Device* device, 
		ID3D11DeviceContext* context, 
		std::wstring filename,
		Model3D& MD5Model,
		TextureManager* textureManager);

	Model3D			MD5Model;
	Transform*		_transform;
};

