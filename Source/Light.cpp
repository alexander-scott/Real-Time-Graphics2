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

void Light::SetLookAt(float x, float y, float z)
{
	_lookAt.x = x;
	_lookAt.y = y;
	_lookAt.z = z;
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

void Light::GenerateViewMatrix()
{
	XMFLOAT3 up;
	XMVECTOR posVec, lookVec, upVec;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVec = XMLoadFloat3(&up);
	lookVec = XMLoadFloat3(&_lookAt);
	posVec = XMLoadFloat3(&_transform->GetPositionValue());

	_viewMatrix = XMMatrixLookAtLH(posVec, lookVec, upVec);
	return;
}

void Light::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;

	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	_projMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	return;
}

void Light::GetViewMatrix(XMMATRIX & viewMat)
{
	viewMat = _viewMatrix;
	return;
}

void Light::GetProjectionMatrix(XMMATRIX & projMat)
{
	projMat = _projMatrix;
	return;
}
