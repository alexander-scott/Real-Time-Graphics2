#include "Transform.h"

Transform::Transform()
{
	_positionX = 0.0f;
	_positionY = 0.0f;
	_positionZ = 0.0f;
	
	_rotationX = 0.0f;
	_rotationY = 0.0f;
	_rotationZ = 0.0f;

	_frameTime = 0.0f;

	_forwardSpeed   = 0.0f;
	_backwardSpeed  = 0.0f;
	_upwardSpeed    = 0.0f;
	_downwardSpeed  = 0.0f;
	_leftTurnSpeed  = 0.0f;
	_rightTurnSpeed = 0.0f;
	_lookUpSpeed    = 0.0f;
	_lookDownSpeed  = 0.0f;
}

Transform::Transform(const Transform& other)
{
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	_positionX = x;
	_positionY = y;
	_positionZ = z;
	return;
}

void Transform::SetRotation(float x, float y, float z)
{
	_rotationX = x;
	_rotationY = y;
	_rotationZ = z;
	return;
}

void Transform::GetPosition(float& x, float& y, float& z)
{
	x = _positionX;
	y = _positionY;
	z = _positionZ;
	return;
}

void Transform::GetRotation(float& x, float& y, float& z)
{
	x = _rotationX;
	y = _rotationY;
	z = _rotationZ;
	return;
}

void Transform::SetFrameTime(float time)
{
	_frameTime = time;
	return;
}

void Transform::MoveForward(bool keydown)
{
	float radians;

	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_forwardSpeed += _frameTime * 1.0f;
		if(_forwardSpeed > (_frameTime * 50.0f))
		{
			_forwardSpeed = _frameTime * 50.0f;
		}
	}
	else
	{
		_forwardSpeed -= _frameTime * 0.5f;

		if(_forwardSpeed < 0.0f)
		{
			_forwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = _rotationY * 0.0174532925f;

	// Update the Position.
	_positionX += sinf(radians) * _forwardSpeed;
	_positionZ += cosf(radians) * _forwardSpeed;

	return;
}

void Transform::MoveBackward(bool keydown)
{
	float radians;

	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_backwardSpeed += _frameTime * 1.0f;

		if(_backwardSpeed > (_frameTime * 50.0f))
		{
			_backwardSpeed = _frameTime * 50.0f;
		}
	}
	else
	{
		_backwardSpeed -= _frameTime * 0.5f;
		
		if(_backwardSpeed < 0.0f)
		{
			_backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = _rotationY * 0.0174532925f;

	// Update the Position.
	_positionX -= sinf(radians) * _backwardSpeed;
	_positionZ -= cosf(radians) * _backwardSpeed;

	return;
}

void Transform::MoveUpward(bool keydown)
{
	// Update the upward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_upwardSpeed += _frameTime * 1.5f;

		if(_upwardSpeed > (_frameTime * 15.0f))
		{
			_upwardSpeed = _frameTime * 15.0f;
		}
	}
	else
	{
		_upwardSpeed -= _frameTime * 0.5f;

		if(_upwardSpeed < 0.0f)
		{
			_upwardSpeed = 0.0f;
		}
	}

	// Update the height Position.
	_positionY += _upwardSpeed;

	return;
}

void Transform::MoveDownward(bool keydown)
{
	// Update the downward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_downwardSpeed += _frameTime * 1.5f;

		if(_downwardSpeed > (_frameTime * 15.0f))
		{
			_downwardSpeed = _frameTime * 15.0f;
		}
	}
	else
	{
		_downwardSpeed -= _frameTime * 0.5f;

		if(_downwardSpeed < 0.0f)
		{
			_downwardSpeed = 0.0f;
		}
	}

	// Update the height Position.
	_positionY -= _downwardSpeed;

	return;
}

void Transform::TurnLeft(bool keydown)
{
	// Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_leftTurnSpeed += _frameTime * 5.0f;

		if(_leftTurnSpeed > (_frameTime * 150.0f))
		{
			_leftTurnSpeed = _frameTime * 150.0f;
		}
	}
	else
	{
		_leftTurnSpeed -= _frameTime* 3.5f;

		if(_leftTurnSpeed < 0.0f)
		{
			_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	_rotationY -= _leftTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(_rotationY < 0.0f)
	{
		_rotationY += 360.0f;
	}

	return;
}

void Transform::TurnRight(bool keydown)
{
	// Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_rightTurnSpeed += _frameTime * 5.0f;

		if(_rightTurnSpeed > (_frameTime * 150.0f))
		{
			_rightTurnSpeed = _frameTime * 150.0f;
		}
	}
	else
	{
		_rightTurnSpeed -= _frameTime* 3.5f;

		if(_rightTurnSpeed < 0.0f)
		{
			_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation.
	_rotationY += _rightTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(_rotationY > 360.0f)
	{
		_rotationY -= 360.0f;
	}

	return;
}

void Transform::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_lookUpSpeed += _frameTime * 7.5f;

		if(_lookUpSpeed > (_frameTime * 75.0f))
		{
			_lookUpSpeed = _frameTime * 75.0f;
		}
	}
	else
	{
		_lookUpSpeed -= _frameTime* 2.0f;

		if(_lookUpSpeed < 0.0f)
		{
			_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	_rotationX -= _lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if(_rotationX > 90.0f)
	{
		_rotationX = 90.0f;
	}

	return;
}

void Transform::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		_lookDownSpeed += _frameTime * 7.5f;

		if(_lookDownSpeed > (_frameTime * 75.0f))
		{
			_lookDownSpeed = _frameTime * 75.0f;
		}
	}
	else
	{
		_lookDownSpeed -= _frameTime* 2.0f;

		if(_lookDownSpeed < 0.0f)
		{
			_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	_rotationX += _lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if(_rotationX < -90.0f)
	{
		_rotationX = -90.0f;
	}

	return;
}