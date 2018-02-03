#pragma once

#include <directxmath.h>
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
	XMFLOAT3 GetDirection();
	XMFLOAT3 GetPosition();

private:
	XMFLOAT4 _ambientColour;
	XMFLOAT4 _diffuseColour;
	XMFLOAT3 _direction;
	XMFLOAT3 _position;
};