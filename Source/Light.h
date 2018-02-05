#pragma once

#include <directxmath.h>

#include "Transform.h"

using namespace DirectX;

class Light
{
public:
	Light();
	Light(const Light&);
	~Light();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetPosition(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();

	Transform* GetTransform() { return _transform; }

private:
	XMFLOAT4		_ambientColour;
	XMFLOAT4		_diffuseColour;

	Transform*		_transform;
};