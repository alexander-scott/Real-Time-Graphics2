#include "Window.h"

Window::Window()
{
	_application = 0;
}

Window::Window(const Window& other)
{
}

Window::~Window()
{
}

bool Window::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the application wrapper object.
	_application = new Application;
	if (!_application)
	{
		return false;
	}

	// Initialize the application wrapper object.
	result = _application->Initialize(_hinstance, _hwnd, screenWidth, screenHeight);
	if (!result)
	{
		return false;
	}

	return true;
}

void Window::Destroy()
{
	// Release the application wrapper object.
	if (_application)
	{
		_application->Destroy();
		delete _application;
		_application = 0;
	}

	// Destroy the window.
	DestroyWindows();
	
	return;
}

void Window::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Update();
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool Window::Update()
{
	bool result;

	// Do the frame processing for the application object.
	result = _application->Update();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK Window::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void Window::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wcex;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	_applicationName = L"DX11 Framework";

	// Setup the windows class with default settings.
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = _hinstance;
	wcex.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wcex.hIconSm       = wcex.hIcon;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = _applicationName;
	wcex.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wcex);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the Position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 1280x720 resolution.
		screenWidth  = 1280;
		screenHeight = 720;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName, NULL,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, _hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void Window::DestroyWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(_hwnd);
	_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(_applicationName, _hinstance);
	_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}