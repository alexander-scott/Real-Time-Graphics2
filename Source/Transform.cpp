#include "Transform.h"

Transform::Transform()
{
	_position = XMFLOAT3();
	_rotation = XMFLOAT3();

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

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	_position = XMFLOAT3(x, y, z);
	return;
}

void Transform::SetRotation(float x, float y, float z)
{
	_rotation = XMFLOAT3(x, y, z);
	return;
}

void Transform::SetScale(float x, float y, float z)
{
	_scale = XMFLOAT3(x, y, z);
	return;
}

void Transform::SetPosition(XMFLOAT3 pos)
{
	_position = pos;
	return;
}

void Transform::SetRotation(XMFLOAT3 rot)
{
	_rotation = rot;
	return;
}

void Transform::SetScale(XMFLOAT3 scale)
{
	_scale = scale;
	return;
}

void Transform::GetPosition(float& x, float& y, float& z)
{
	x = _position.x;
	y = _position.y;
	z = _position.z;
	return;
}

void Transform::GetRotation(float& x, float& y, float& z)
{
	x = _rotation.x;
	y = _rotation.y;
	z = _rotation.z;
	return;
}

void Transform::GetScale(float & x, float & y, float & z)
{
	x = _scale.x;
	y = _scale.y;
	z = _scale.z;
	return;
}

void Transform::GetPosition(XMFLOAT3 & pos)
{
	pos = _position;
	return;
}

void Transform::GetRotation(XMFLOAT3 & rot)
{
	rot = _rotation;
	return;
}

void Transform::GetScale(XMFLOAT3 & scale)
{
	scale = _scale;
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
	radians = _rotation.y * 0.0174532925f;

	// Update the Position.
	_position.x += sinf(radians) * _forwardSpeed;
	_position.z += cosf(radians) * _forwardSpeed;

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
	radians = _rotation.y * 0.0174532925f;

	// Update the Position.
	_position.x -= sinf(radians) * _backwardSpeed;
	_position.z -= cosf(radians) * _backwardSpeed;

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
	_position.y += _upwardSpeed;

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
	_position.y -= _downwardSpeed;

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
	_rotation.y -= _leftTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(_rotation.y < 0.0f)
	{
		_rotation.y += 360.0f;
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
	_rotation.y += _rightTurnSpeed;

	// Keep the rotation in the 0 to 360 range.
	if(_rotation.y > 360.0f)
	{
		_rotation.y -= 360.0f;
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
	_rotation.x -= _lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if(_rotation.x > 90.0f)
	{
		_rotation.x = 90.0f;
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
	_rotation.x += _lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if(_rotation.x < -90.0f)
	{
		_rotation.x = -90.0f;
	}

	return;
}