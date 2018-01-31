#include "Window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Window* window;
	bool result;
	
	// Create the system object.
	window = new Window;
	if(!window)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = window->Initialize();
	if(result)
	{
		window->Run();
	}

	// Shutdown and release the system object.
	window->Shutdown();
	delete window;
	window = nullptr;

	return 0;
}