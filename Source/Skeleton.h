#pragma once

#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>

#include <string>
#include <vector>

#include "TextureManager.h"
#include "Transform.h"

using namespace DirectX;

class Skeleton
{
private:
	struct Vertex    //Overloaded Vertex Structure
	{
		Vertex() {}
		Vertex(float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz,
			float tx, float ty, float tz)
			: Pos(x, y, z), TexCoord(u, v), Normal(nx, ny, nz),
			Tangent(tx, ty, tz) {}

		XMFLOAT3 Pos;
		XMFLOAT2 TexCoord;
		XMFLOAT3 Normal;
		XMFLOAT3 Tangent;
		XMFLOAT3 BiTangent;

		// Will not be sent to shader
		int StartWeight;
		int WeightCount;
	};

	struct Joint
	{
		std::wstring Name;
		int ParentID;

		XMFLOAT3 Postion;
		XMFLOAT4 Orientation;
	};

	struct Weight
	{
		int JointID;
		float Bias;
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
	};

	struct ModelSubset
	{
		int TexArrayIndex;
		int NumTriangles;

		std::vector<Vertex> Vertices;
		std::vector<DWORD> Indices;
		std::vector<Weight> Weights;

		std::vector<XMFLOAT3> Positions;

		ID3D11Buffer* VertBuff;
		ID3D11Buffer* IndexBuff;
	};

	struct BoundingBox
	{
		XMFLOAT3 Min;
		XMFLOAT3 Max;
	};

	struct FrameData
	{
		int FrameID;
		std::vector<float> FrameDataVec;
	};

	struct AnimJointInfo
	{
		std::wstring Name;
		int ParentID;

		int Flags;
		int StartIndex;
	};

	struct ModelAnimation
	{
		int NumFrames;
		int NumJoints;
		int FrameRate;
		int NumAnimatedComponents;

		float FrameTime;
		float TotalAnimTime;
		float CurrAnimTime;

		std::vector<AnimJointInfo> JointInfo;
		std::vector<BoundingBox> FrameBounds;
		std::vector<Joint>    BaseFrameJoints;
		std::vector<FrameData>    FrameData;
		std::vector<std::vector<Joint>> FrameSkeleton;
	};

	struct Model3D
	{
		int NumSubsets;
		int NumJoints;

		std::vector<Joint> Joints;
		std::vector<ModelSubset> Subsets;
		std::vector<ModelAnimation> Animations;
	};

public:
	Skeleton();
	~Skeleton();

	bool Initialize(ID3D11Device * device, ID3D11DeviceContext* context, TextureManager* textureManager, std::wstring meshFileName, std::wstring animFileName);
	void Destroy();

	void Update(ID3D11DeviceContext* context, float deltaTime);
	void Draw(ID3D11DeviceContext* deviceContext);
	void DrawSubset(ID3D11DeviceContext* deviceContext, int index);

	int GetIndexCount(int index);
	int GetSubsetCount() { return _md5Model.NumSubsets; }

	Transform* GetTransform() { return _transform; }

private:
	bool LoadMD5Model(ID3D11Device* device, 
		ID3D11DeviceContext* context, 
		std::wstring filename,
		Model3D& MD5Model,
		TextureManager* textureManager);

	bool LoadMD5Anim(std::wstring filename, Model3D& MD5Model);

	Model3D			_md5Model;
	Transform*		_transform;
};

