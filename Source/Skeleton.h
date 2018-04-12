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
	XMFLOAT3 normal;
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

struct BoundingBox
{
	XMFLOAT3 min;
	XMFLOAT3 max;
};

struct FrameData
{
	int frameID;
	std::vector<float> frameData;
};
struct AnimJointInfo
{
	std::wstring name;
	int parentID;

	int flags;
	int startIndex;
};

struct ModelAnimation
{
	int numFrames;
	int numJoints;
	int frameRate;
	int numAnimatedComponents;

	float frameTime;
	float totalAnimTime;
	float currAnimTime;

	std::vector<AnimJointInfo> jointInfo;
	std::vector<BoundingBox> frameBounds;
	std::vector<Joint>    baseFrameJoints;
	std::vector<FrameData>    frameData;
	std::vector<std::vector<Joint>> frameSkeleton;
};

struct Model3D
{
	int numSubsets;
	int numJoints;

	std::vector<Joint> joints;
	std::vector<ModelSubset> subsets;
	std::vector<ModelAnimation> animations;
};

class Skeleton
{
public:
	Skeleton();
	~Skeleton();

	bool Initialize(ID3D11Device * device, ID3D11DeviceContext* context, TextureManager* textureManager, std::wstring meshFileName, std::wstring animFileName);
	void Destroy();

	void Update(ID3D11DeviceContext* context, float deltaTime);
	void Draw(ID3D11DeviceContext* deviceContext);
	void DrawSubset(ID3D11DeviceContext* deviceContext, int index);

	int GetIndexCount(int index);
	int GetSubsetCount() { return MD5Model.numSubsets; }

	Transform* GetTransform() { return _transform; }

private:
	bool LoadMD5Model(ID3D11Device* device, 
		ID3D11DeviceContext* context, 
		std::wstring filename,
		Model3D& MD5Model,
		TextureManager* textureManager);

	bool LoadMD5Anim(std::wstring filename, Model3D& MD5Model);

	Model3D			MD5Model;
	Transform*		_transform;
};

