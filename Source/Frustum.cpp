#include "Frustum.h"

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Frustum::Initialize(float screenDepth)
{
	_screenDepth = screenDepth;
	return;
}

void Frustum::ConstructFrustum(XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	XMFLOAT4X4 pMatrix, matrix;
	float zMinimum, r, length;
	XMMATRIX finalMatrix;

	// Convert the projection matrix into a 4x4 float type.
	XMStoreFloat4x4(&pMatrix, projectionMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -pMatrix._43 / pMatrix._33;
	r = _screenDepth / (_screenDepth - zMinimum);

	// Load the updated values back into the projection matrix.
	pMatrix._33 = r;
	pMatrix._43 = -r * zMinimum;
	projectionMatrix = XMLoadFloat4x4(&pMatrix);

	// Create the frustum matrix from the view matrix and updated projection matrix.
	finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	// Convert the final matrix into a 4x4 float type.
	XMStoreFloat4x4(&matrix, finalMatrix);

	// Calculate near plane of frustum.
	_planes[0][0] = matrix._14 + matrix._13;
	_planes[0][1] = matrix._24 + matrix._23;
	_planes[0][2] = matrix._34 + matrix._33;
	_planes[0][3] = matrix._44 + matrix._43;

	// Normalize the near plane.
	length = sqrtf((_planes[0][0] * _planes[0][0]) + (_planes[0][1] * _planes[0][1]) + (_planes[0][2] * _planes[0][2]));
	_planes[0][0] /= length;
	_planes[0][1] /= length;
	_planes[0][2] /= length;
	_planes[0][3] /= length;

	// Calculate far plane of frustum.
	_planes[1][0] = matrix._14 - matrix._13;
	_planes[1][1] = matrix._24 - matrix._23;
	_planes[1][2] = matrix._34 - matrix._33;
	_planes[1][3] = matrix._44 - matrix._43;

	// Normalize the far plane.
	length = sqrtf((_planes[1][0] * _planes[1][0]) + (_planes[1][1] * _planes[1][1]) + (_planes[1][2] * _planes[1][2]));
	_planes[1][0] /= length;
	_planes[1][1] /= length;
	_planes[1][2] /= length;
	_planes[1][3] /= length;

	// Calculate left plane of frustum.
	_planes[2][0] = matrix._14 + matrix._11;
	_planes[2][1] = matrix._24 + matrix._21;
	_planes[2][2] = matrix._34 + matrix._31;
	_planes[2][3] = matrix._44 + matrix._41;

	// Normalize the left plane.
	length = sqrtf((_planes[2][0] * _planes[2][0]) + (_planes[2][1] * _planes[2][1]) + (_planes[2][2] * _planes[2][2]));
	_planes[2][0] /= length;
	_planes[2][1] /= length;
	_planes[2][2] /= length;
	_planes[2][3] /= length;

	// Calculate right plane of frustum.
	_planes[3][0] = matrix._14 - matrix._11;
	_planes[3][1] = matrix._24 - matrix._21;
	_planes[3][2] = matrix._34 - matrix._31;
	_planes[3][3] = matrix._44 - matrix._41;

	// Normalize the right plane.
	length = sqrtf((_planes[3][0] * _planes[3][0]) + (_planes[3][1] * _planes[3][1]) + (_planes[3][2] * _planes[3][2]));
	_planes[3][0] /= length;
	_planes[3][1] /= length;
	_planes[3][2] /= length;
	_planes[3][3] /= length;

	// Calculate top plane of frustum.
	_planes[4][0] = matrix._14 - matrix._12;
	_planes[4][1] = matrix._24 - matrix._22;
	_planes[4][2] = matrix._34 - matrix._32;
	_planes[4][3] = matrix._44 - matrix._42;

	// Normalize the top plane.
	length = sqrtf((_planes[4][0] * _planes[4][0]) + (_planes[4][1] * _planes[4][1]) + (_planes[4][2] * _planes[4][2]));
	_planes[4][0] /= length;
	_planes[4][1] /= length;
	_planes[4][2] /= length;
	_planes[4][3] /= length;

	// Calculate bottom plane of frustum.
	_planes[5][0] = matrix._14 + matrix._12;
	_planes[5][1] = matrix._24 + matrix._22;
	_planes[5][2] = matrix._34 + matrix._32;
	_planes[5][3] = matrix._44 + matrix._42;

	// Normalize the bottom plane.
	length = sqrtf((_planes[5][0] * _planes[5][0]) + (_planes[5][1] * _planes[5][1]) + (_planes[5][2] * _planes[5][2]));
	_planes[5][0] /= length;
	_planes[5][1] /= length;
	_planes[5][2] /= length;
	_planes[5][3] /= length;

	return;
}

bool Frustum::CheckPoint(float x, float y, float z)
{
	int i;
	float dotProduct;

	// Check each of the six planes to make sure the point is inside all of them and hence inside the frustum.
	for (i = 0; i<6; i++)
	{
		// Calculate the dot product of the plane and the 3D point.
		dotProduct = (_planes[i][0] * x) + (_planes[i][1] * y) + (_planes[i][2] * z) + (_planes[i][3] * 1.0f);

		// Determine if the point is on the correct side of the current plane, exit out if it is not.
		if (dotProduct <= 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	float dotProduct;

	// Check each of the six planes to see if the cube is inside the frustum.
	for (i = 0; i<6; i++)
	{
		// Check all eight points of the cube to see if they all reside within the frustum.
		dotProduct = (_planes[i][0] * (xCenter - radius)) + (_planes[i][1] * (yCenter - radius)) + (_planes[i][2] * (zCenter - radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + radius)) + (_planes[i][1] * (yCenter - radius)) + (_planes[i][2] * (zCenter - radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter - radius)) + (_planes[i][1] * (yCenter + radius)) + (_planes[i][2] * (zCenter - radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + radius)) + (_planes[i][1] * (yCenter + radius)) + (_planes[i][2] * (zCenter - radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter - radius)) + (_planes[i][1] * (yCenter - radius)) + (_planes[i][2] * (zCenter + radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + radius)) + (_planes[i][1] * (yCenter - radius)) + (_planes[i][2] * (zCenter + radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter - radius)) + (_planes[i][1] * (yCenter + radius)) + (_planes[i][2] * (zCenter + radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + radius)) + (_planes[i][1] * (yCenter + radius)) + (_planes[i][2] * (zCenter + radius)) + (_planes[i][3] * 1.0f);
		if (dotProduct > 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;
	float dotProduct;

	// Check the six planes to see if the sphere is inside them or not.
	for (i = 0; i<6; i++)
	{
		dotProduct = ((_planes[i][0] * xCenter) + (_planes[i][1] * yCenter) + (_planes[i][2] * zCenter) + (_planes[i][3] * 1.0f));
		if (dotProduct <= -radius)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;
	float dotProduct;

	// Check each of the six planes to see if the rectangle is in the frustum or not.
	for (i = 0; i<6; i++)
	{
		dotProduct = (_planes[i][0] * (xCenter - xSize)) + (_planes[i][1] * (yCenter - ySize)) + (_planes[i][2] * (zCenter - zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + xSize)) + (_planes[i][1] * (yCenter - ySize)) + (_planes[i][2] * (zCenter - zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter - xSize)) + (_planes[i][1] * (yCenter + ySize)) + (_planes[i][2] * (zCenter - zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + xSize)) + (_planes[i][1] * (yCenter + ySize)) + (_planes[i][2] * (zCenter - zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter - xSize)) + (_planes[i][1] * (yCenter - ySize)) + (_planes[i][2] * (zCenter + zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + xSize)) + (_planes[i][1] * (yCenter - ySize)) + (_planes[i][2] * (zCenter + zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter - xSize)) + (_planes[i][1] * (yCenter + ySize)) + (_planes[i][2] * (zCenter + zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = (_planes[i][0] * (xCenter + xSize)) + (_planes[i][1] * (yCenter + ySize)) + (_planes[i][2] * (zCenter + zSize)) + (_planes[i][3] * 1.0f);
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool Frustum::CheckRectangle2(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth)
{
	int i;
	float dotProduct;

	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (i = 0; i<6; i++)
	{
		dotProduct = ((_planes[i][0] * minWidth) + (_planes[i][1] * minHeight) + (_planes[i][2] * minDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((_planes[i][0] * maxWidth) + (_planes[i][1] * minHeight) + (_planes[i][2] * minDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((_planes[i][0] * minWidth) + (_planes[i][1] * maxHeight) + (_planes[i][2] * minDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((_planes[i][0] * maxWidth) + (_planes[i][1] * maxHeight) + (_planes[i][2] * minDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((_planes[i][0] * minWidth) + (_planes[i][1] * minHeight) + (_planes[i][2] * maxDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((_planes[i][0] * maxWidth) + (_planes[i][1] * minHeight) + (_planes[i][2] * maxDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((_planes[i][0] * minWidth) + (_planes[i][1] * maxHeight) + (_planes[i][2] * maxDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((_planes[i][0] * maxWidth) + (_planes[i][1] * maxHeight) + (_planes[i][2] * maxDepth) + (_planes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}
