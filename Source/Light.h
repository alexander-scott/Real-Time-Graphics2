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

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();

	Transform* GetTransform() { return _transform; }

private:
	XMFLOAT4		_ambientColour;
	XMFLOAT4		_diffuseColour;

	Transform*		_transform;
};