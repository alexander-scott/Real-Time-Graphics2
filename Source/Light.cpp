#include "Light.h"

Light::Light()
{
	_transform = new Transform;
}

Light::~Light()
{
}

void Light::SetAmbientColor(float red, float green, float blue, float alpha)
{
	_ambientColour = XMFLOAT4(red, green, blue, alpha);
	return;
}

void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	_diffuseColour = XMFLOAT4(red, green, blue, alpha);
	return;
}


void Light::SetDirection(float x, float y, float z)
{
	_transform->SetRotation(XMFLOAT3(x, y, z));
	return;
}

void Light::SetPosition(float x, float y, float z)
{
	_transform->SetPosition(XMFLOAT3(x, y, z));
	return;
}

XMFLOAT4 Light::GetAmbientColor()
{
	return _ambientColour;
}

XMFLOAT4 Light::GetDiffuseColor()
{
	return _diffuseColour;
}