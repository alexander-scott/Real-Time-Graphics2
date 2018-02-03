#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_

#include <math.h>

class Transform
{
public:
	Transform();
	Transform(const Transform&);
	~Transform();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	void SetFrameTime(float);

	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveUpward(bool);
	void MoveDownward(bool);
	void TurnLeft(bool);
	void TurnRight(bool);
	void LookUpward(bool);
	void LookDownward(bool);

private:
	float _positionX, _positionY, _positionZ;
	float _rotationX, _rotationY, _rotationZ;

	float _frameTime;

	float _forwardSpeed, _backwardSpeed;
	float _upwardSpeed, _downwardSpeed;
	float _leftTurnSpeed, _rightTurnSpeed;
	float _lookUpSpeed, _lookDownSpeed;
};

#endif