#include "Camera.h"

Camera::Camera()
{
	_transform = new Transform;

	_transform->SetPosition(0, 0, 0);
	_transform->SetRotation(0, 0, 0);
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
	delete _transform;
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the Position of the camera in the World.
	_transform->GetPosition(position.x, position.y, position.z);

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	float rotationX, rotationY, rotationZ = 0;
	_transform->GetRotation(rotationX, rotationY, rotationZ);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX * 0.0174532925f;
	yaw   = rotationY * 0.0174532925f;
	roll  = rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the View is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera Position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the View matrix from the three updated vectors.
	_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = _viewMatrix;
	return;
}

void Camera::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Load it into a XMVECTOR structure.
	upVector = XMLoadFloat3(&up);

	// Setup the Position of the camera in the World.
	_transform->GetPosition(position.x, position.y, position.z);

	// Load it into a XMVECTOR structure.
	positionVector = XMLoadFloat3(&position);

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Load it into a XMVECTOR structure.
	lookAtVector = XMLoadFloat3(&lookAt);

	float rotationX, rotationY, rotationZ = 0;
	_transform->GetRotation(rotationX, rotationY, rotationZ);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX * 0.0174532925f;
	yaw = rotationY * 0.0174532925f;
	roll = rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the View is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera Position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the View matrix from the three updated vectors.
	_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Camera::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = _baseViewMatrix;
	return;
}