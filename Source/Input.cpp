#include "Input.h"

Input::Input()
{
	_directInput = nullptr;
	_keyboard = nullptr;
	_mouse = nullptr;
}

Input::Input(const Input& other)
{
}

Input::~Input()
{
}

bool Input::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	_mouseX = 0;
	_mouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = _directInput->CreateDevice(GUID_SysKeyboard, &_keyboard, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = _keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to share with other programs.
	result = _keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = _keyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = _directInput->CreateDevice(GUID_SysMouse, &_mouse, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = _mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = _mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = _mouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	_f1Released = true;
	_f2Released = true;

	return true;
}

void Input::Destroy()
{
	// Release the mouse.
	if (_mouse)
	{
		_mouse->Unacquire();
		_mouse->Release();
		_mouse = 0;
	}

	// Release the keyboard.
	if (_keyboard)
	{
		_keyboard->Unacquire();
		_keyboard->Release();
		_keyboard = 0;
	}

	// Release the main interface to direct input.
	if (_directInput)
	{
		_directInput->Release();
		_directInput = 0;
	}

	return;
}

bool Input::Update()
{
	bool result;


	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

bool Input::ReadKeyboard()
{
	HRESULT result;


	// Read the keyboard device.
	result = _keyboard->GetDeviceState(sizeof(_keyboardState), (LPVOID)&_keyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Input::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = _mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_mouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Input::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	_mouseX += _mouseState.lX;
	_mouseY += _mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (_mouseX < 0) { _mouseX = 0; }
	if (_mouseY < 0) { _mouseY = 0; }

	if (_mouseX > _screenWidth) { _mouseX = _screenWidth; }
	if (_mouseY > _screenHeight) { _mouseY = _screenHeight; }

	return;
}

bool Input::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void Input::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = _mouseX;
	mouseY = _mouseY;
	return;
}

bool Input::IsLeftPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_LEFT] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsRightPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_RIGHT] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_UP] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_DOWN] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsAPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_A] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsZPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_Z] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsPgUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_PGUP] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsPgDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if (_keyboardState[DIK_PGDN] & 0x80)
	{
		return true;
	}

	return false;
}

bool Input::IsF1Toggled()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(_keyboardState[DIK_F1] & 0x80)
	{
		if(_f1Released)
		{
			_f1Released = false;
			return true;
		}
	}
	else
	{
		_f1Released = true;
	}

	return false;
}

bool Input::IsF2Toggled()
{
	// Do a bitwise and on the keyboard state to check if the key is currently being pressed.
	if(_keyboardState[DIK_F2] & 0x80)
	{
		if(_f2Released)
		{
			_f2Released = false;
			return true;
		}
	}
	else
	{
		_f2Released = true;
	}

	return false;
}