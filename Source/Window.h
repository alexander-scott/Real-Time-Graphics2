#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Application.h"

class Window
{
public:
	Window();
	~Window();

	bool Initialize();
	void Destroy();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	void InitializeWindows(int&, int&);
	void DestroyWindows();

private:
	LPCWSTR			_applicationName;
	HINSTANCE		_hinstance;
	HWND			_hwnd;
	Application*	_application;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Window* ApplicationHandle = nullptr;

#endif