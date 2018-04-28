#pragma once

#include <directxmath.h>

using namespace DirectX;

class Frustum
{
public:
	Frustum();
	~Frustum();

	void Initialize(float);

	void ConstructFrustum(XMMATRIX projectionMatrix, XMMATRIX viewMatrix);

	bool CheckPoint(float x, float y, float z);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);
	bool CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);

private:
	float	_screenDepth;
	float	_planes[6][4];
};