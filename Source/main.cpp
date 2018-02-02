#include "Window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Window* window;
	bool result;
	
	// Create the window object.
	window = new Window;
	if(!window)
	{
		return 0;
	}

	// Initialize and run the window object.
	result = window->Initialize();
	if(result)
	{
		window->Run();
	}

	// Destroy and release the window object.
	window->Destroy();
	delete window;
	window = nullptr;

	return 0;
}