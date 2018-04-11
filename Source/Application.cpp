#include "Application.h"

Application::Application()
{
	_input = nullptr;
	_dx11Instance = nullptr;
	_timer = nullptr;
	_shaderManager = nullptr;
	_textureManager = nullptr;
	_scene = nullptr;
}

Application::Application(const Application& other)
{
}

Application::~Application()
{
}

bool Application::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;
	
	// Create the input object.
	_input = new Input;
	if (!_input)
	{
		return false;
	}

	// Initialize the input object.
	result = _input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	_dx11Instance = new DX11Instance;
	if(!_dx11Instance)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = _dx11Instance->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the shader manager object.
	_shaderManager = new ShaderManager;
	if(!_shaderManager)
	{
		return false;
	}

	// Initialize the shader manager object.
	result = _shaderManager->Initialize(_dx11Instance->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}

	// Create the timer object.
	_timer = new Timer;
	if(!_timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = _timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the TargaTexture manager object.
	_textureManager = new TextureManager;
	if (!_textureManager)
	{
		return false;
	}

	// Initialize the TargaTexture manager object.
	result = _textureManager->Initialize(10, 10);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture manager object.", L"Error", MB_OK);
		return false;
	}

	switch (CURRENT_SCENE)
	{
		case Scene::eSceneTerrainLOD:
			return BuildSceneTerrainLOD(hwnd, screenWidth, screenHeight);
	}

	return true;
}

void Application::Destroy()
{
	// Release the scene object.
	if(_scene)
	{
		_scene->Destroy();
		delete _scene;
		_scene = 0;
	}

	// Release the timer object.
	if(_timer)
	{
		delete _timer;
		_timer = 0;
	}

	// Release the shader manager object.
	if(_shaderManager)
	{
		_shaderManager->Destroy();
		delete _shaderManager;
		_shaderManager = 0;
	}

	// Release the TargaTexture manager object.
	if (_textureManager)
	{
		_textureManager->Destroy();
		delete _textureManager;
		_textureManager = 0;
	}

	// Release the dx11 object.
	if(_dx11Instance)
	{
		_dx11Instance->Destroy();
		delete _dx11Instance;
		_dx11Instance = 0;
	}

	// Release the input object.
	if(_input)
	{
		_input->Destroy();
		delete _input;
		_input = 0;
	}

	return;
}

bool Application::Update()
{
	bool result;

	// Update the system stats.
	_timer->Update();

	// Do the input frame processing.
	result = _input->Update();
	if(!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if(_input->IsEscapePressed() == true)
	{
		return false;
	}

	// Do the scene frame processing.
	result = _scene->Update(_dx11Instance, _input, _shaderManager, _textureManager, _timer->GetTime());
	if (!result)
	{
		return false;
	}

	return result;
}

bool Application::BuildSceneTerrainLOD(HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;

	result = _textureManager->LoadTargaTexture(_dx11Instance->GetDevice(), _dx11Instance->GetDeviceContext(), "Source/terrain/rock01d.tga", 0);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadTargaTexture(_dx11Instance->GetDevice(), _dx11Instance->GetDeviceContext(), "Source/terrain/rock01n.tga", 1);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadTargaTexture(_dx11Instance->GetDevice(), _dx11Instance->GetDeviceContext(), "Source/terrain/snow01n.tga", 2);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadTargaTexture(_dx11Instance->GetDevice(), _dx11Instance->GetDeviceContext(), "Source/terrain/distance01n.tga", 3);
	if (!result)
	{
		return false;
	}

	// Create the scene object.
	_scene = new SceneTerrainLOD;
	if (!_scene)
	{
		return false;
	}

	// Initialize the scene object.
	result = _scene->Initialize(_dx11Instance, hwnd, screenWidth, screenHeight, SCREEN_DEPTH);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the zone object.", L"Error", MB_OK);
		return false;
	}

	return result;
}
