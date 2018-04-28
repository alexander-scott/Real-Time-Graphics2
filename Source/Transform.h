#pragma once

#include <math.h>
#include <directxmath.h>

using namespace DirectX;

class Transform
{
public:
	Transform();
	~Transform();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	void SetPosition(XMFLOAT3 pos);
	void SetRotation(XMFLOAT3 rot);
	void SetScale(XMFLOAT3 scale);

	void GetPosition(float& x, float& y, float& z);
	void GetRotation(float& x, float& y, float& z);
	void GetScale(float& x, float& y, float& z);

	void GetPosition(XMFLOAT3& pos);
	void GetRotation(XMFLOAT3& rot);
	void GetScale(XMFLOAT3& scale);

	XMFLOAT3& GetPositionValue() { return _position; }
	XMFLOAT3& GetRotationValue() { return _rotation; }
	XMFLOAT3& GetScaleValue() { return _scale; }

	void SetFrameTime(float deltaTime);

	void MoveForward(bool keydown);
	void MoveBackward(bool keydown);
	void MoveUpward(bool keydown);
	void MoveDownward(bool keydown);
	void TurnLeft(bool keydown);
	void TurnRight(bool keydown);
	void LookUpward(bool keydown);
	void LookDownward(bool keydown);

private:
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	float _frameTime;

	float _forwardSpeed, _backwardSpeed;
	float _upwardSpeed, _downwardSpeed;
	float _leftTurnSpeed, _rightTurnSpeed;
	float _lookUpSpeed, _lookDownSpeed;
};