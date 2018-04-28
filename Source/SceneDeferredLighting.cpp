#include "SceneDeferredLighting.h"

SceneDeferredLighting::SceneDeferredLighting()
{
	_light = 0;
	_cube = 0;
	_window = 0;
	_deferredBuffers = 0;
}

SceneDeferredLighting::SceneDeferredLighting(const SceneDeferredLighting& other)
{
}

bool SceneDeferredLighting::Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
{
	bool result;

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

	result = _textureManager->LoadJPEGTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), L"Source/skydome/seafloor.dds", 47);
	if (!result)
	{
		return false;
	}

	// Create the camera object.
	_camera = new Camera;
	if (!_camera)
	{
		return false;
	}

	// Set the initial Position and rotation.
	_camera->GetTransform()->SetPosition(0.0f, 0.0f, -10.0f);
	_camera->GetTransform()->SetRotation(0.0f, 0.0f, 0.0f);

	// Set the initial Position of the camera and build the matrices needed for rendering.
	_camera->Render();
	_camera->RenderBaseViewMatrix();

	// Create the frustum object.
	_frustum = new Frustum;
	if (!_frustum)
	{
		return false;
	}

	// Initialize the frustum object.
	_frustum->Initialize(screenDepth);

	// Create the light object.
	_light = new Light;
	if (!_light)
	{
		return false;
	}

	// Initialize the light object.
	_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	_light->SetDirection(0.0f, 0.0f, 1.0f);

	// Create the model object.
	_cube = new Cube;
	if (!_cube)
	{
		return false;
	}

	// Initialize the model object.
	result = _cube->Initialize(Direct3D->GetDevice(), "Source/skydome/cube.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the full screen ortho window object.
	_window = new OrthoWindow;
	if (!_window)
	{
		return false;
	}

	// Initialize the full screen ortho window object.
	result = _window->Initialize(Direct3D->GetDevice(), screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	// Create the deferred buffers object.
	_deferredBuffers = new DeferredBuffers;
	if (!_deferredBuffers)
	{
		return false;
	}

	// Initialize the deferred buffers object.
	result = _deferredBuffers->Initialize(Direct3D->GetDevice(), screenWidth, screenHeight, screenDepth, 0.1f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SceneDeferredLighting::Destroy()
{
	// Release the deferred buffers object.
	if (_deferredBuffers)
	{
		_deferredBuffers->Shutdown();
		delete _deferredBuffers;
		_deferredBuffers = 0;
	}

	// Release the full screen ortho window object.
	if (_window)
	{
		_window->Shutdown();
		delete _window;
		_window = 0;
	}

	// Release the model object.
	if (_cube)
	{
		_cube->Shutdown();
		delete _cube;
		_cube = 0;
	}

	// Release the light object.
	if (_light)
	{
		delete _light;
		_light = 0;
	}

	// Release the TargaTexture manager object.
	if (_textureManager)
	{
		_textureManager->Destroy();
		delete _textureManager;
		_textureManager = nullptr;
	}

	// Release the frustum object.
	if (_frustum)
	{
		delete _frustum;
		_frustum = nullptr;
	}

	// Release the camera object.
	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}
}

bool SceneDeferredLighting::Update(DX11Instance * direct3D, Input * input, ShaderManager * shaderManager, float frameTime)
{
	// Do the frame input processing.
	ProcessInput(input, frameTime);

	// Render the graphics.
	bool result = Draw(direct3D, shaderManager, _textureManager);
	if (!result)
	{
		return false;
	}

	return true;
}

void SceneDeferredLighting::ProcessInput(Input * input, float frameTime)
{
	bool keyDown;

	// Set the frame time for calculating the updated Position.
	_camera->GetTransform()->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = input->IsLeftPressed();
	_camera->GetTransform()->TurnLeft(keyDown);

	keyDown = input->IsRightPressed();
	_camera->GetTransform()->TurnRight(keyDown);

	keyDown = input->IsUpPressed();
	_camera->GetTransform()->MoveForward(keyDown);

	keyDown = input->IsDownPressed();
	_camera->GetTransform()->MoveBackward(keyDown);

	keyDown = input->IsAPressed();
	_camera->GetTransform()->MoveUpward(keyDown);

	keyDown = input->IsZPressed();
	_camera->GetTransform()->MoveDownward(keyDown);

	keyDown = input->IsPgUpPressed();
	_camera->GetTransform()->LookUpward(keyDown);

	keyDown = input->IsPgDownPressed();
	_camera->GetTransform()->LookDownward(keyDown);

	return;
}

bool SceneDeferredLighting::Draw(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;
	XMFLOAT3 cameraPosition, cubePosition;

	// Render the scene to the render buffers.
	result = RenderSceneToTexture(direct3D, shaderManager, textureManager);
	if (!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the World, View, and Projection matrices from the camera and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	_camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	direct3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_window->Render(direct3D->GetDeviceContext());

	// Render the full screen ortho window using the deferred light shader and the render buffers.
	shaderManager->RenderDeferredLightShader(direct3D->GetDeviceContext(), _window->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		_deferredBuffers->GetShaderResourceView(0), _deferredBuffers->GetShaderResourceView(1),
		_light->GetTransform()->GetRotationValue());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}

bool SceneDeferredLighting::RenderSceneToTexture(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;

	// Set the render buffers to be the render target.
	_deferredBuffers->SetRenderTargets(direct3D->GetDeviceContext());

	// Clear the render buffers.
	_deferredBuffers->ClearRenderTargets(direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the View matrix based on the camera's Position.
	_camera->Render();

	// Get the World, View, and Projection matrices from the camera and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	_camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);

	// Update the rotation variable each frame.
	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Rotate the world matrix by the rotation value so that the cube will spin.
	worldMatrix *= XMMatrixRotationY(rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	_cube->Render(direct3D->GetDeviceContext());

	// Render the model using the deferred shader.
	shaderManager->RenderDeferredShader(direct3D->GetDeviceContext(), _cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, textureManager->GetTexture(47));

	// Reset the render target back to the original back buffer and not the render buffers anymore.
	direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	direct3D->ResetViewport();

	return true;
}