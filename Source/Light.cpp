#include "Light.h"

Light::Light()
{
}

Light::Light(const Light &)
{
}

Light::~Light()
{
}

void Light::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);
	return;
}

void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
	return;
}


void Light::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
	return;
}

void Light::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	return;
}

XMFLOAT4 Light::GetAmbientColor()
{
	return m_ambientColor;
}

XMFLOAT4 Light::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 Light::GetDirection()
{
	return m_direction;
}

XMFLOAT3 Light::GetPosition()
{
	return m_position;
}
