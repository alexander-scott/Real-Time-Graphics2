#pragma once

#include <directxmath.h>

#include "Transform.h"

using namespace DirectX;

class Light
{
public:
	Light();
	~Light();

	void SetAmbientColor(float red, float green, float blue, float alpha);
	void SetDiffuseColor(float red, float green, float blue, float alpha);
	void SetDirection(float x, float y, float z);
	void SetPosition(float x, float y, float z);

	void SetLookAt(float x, float y, float z);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();

	Transform* GetTransform() { return _transform; }

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenDepth, float screenNear);

	void GetViewMatrix(XMMATRIX& viewMat);
	void GetProjectionMatrix(XMMATRIX& projMat);

private:
	XMFLOAT4		_ambientColour;
	XMFLOAT4		_diffuseColour;

	Transform*		_transform;
	XMFLOAT3		_lookAt;

	XMMATRIX		_viewMatrix;
	XMMATRIX		_projMatrix;
};