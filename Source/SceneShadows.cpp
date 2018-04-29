#include "SceneShadows.h"

SceneShadows::SceneShadows()
{
	_light = 0;
}

bool SceneShadows::Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
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

	result = _textureManager->LoadJPEGTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), L"Source/shadows/wall01.dds", 47);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadJPEGTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), L"Source/shadows/ice.dds", 48);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadJPEGTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), L"Source/shadows/metal001.dds", 49);
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
	_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	_light->SetDirection(0.0f, 0.0f, 1.0f);
	_light->SetLookAt(0.0f, 0.0f, 0.0f);
	_light->GetTransform()->SetPosition(0.0f, 20.0f, -20.0f);
	_light->GenerateProjectionMatrix(screenDepth, 1.0f);

	// Create the model object.
	_cube = new Object;
	if (!_cube)
	{
		return false;
	}

	// Initialize the model object.
	result = _cube->Initialize(Direct3D->GetDevice(), "Source/shadows/cube.txt", 47);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	_cube->GetTransform()->SetPosition(-2.0f, 2.0f, 0.0f);
	_objects.push_back(_cube);

	// Create the model object.
	Object* _plane = new Object;
	if (!_plane)
	{
		return false;
	}

	// Initialize the model object.
	result = _plane->Initialize(Direct3D->GetDevice(), "Source/shadows/plane01.txt", 48);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	_plane->GetTransform()->SetPosition(0.0f, -1.0f, 5.0f);
	_objects.push_back(_plane);

	// Create the model object.
	Object* _sphere = new Object;
	if (!_sphere)
	{
		return false;
	}

	// Initialize the model object.
	result = _sphere->Initialize(Direct3D->GetDevice(), "Source/shadows/sphere.txt", 49);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	_sphere->GetTransform()->SetPosition(2.0f, 2.0f, -3.0f);
	_objects.push_back(_sphere);

	// Create the deferred buffers object.
	_renderTextureBuffer = new RenderTextureBuffer;
	if (!_renderTextureBuffer)
	{
		return false;
	}

	// Initialize the deferred buffers object.
	result = _renderTextureBuffer->Initialize(Direct3D->GetDevice(), screenWidth, screenHeight, screenDepth, 0.1f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SceneShadows::Destroy()
{
	// Release the deferred buffers object.
	if (_renderTextureBuffer)
	{
		_renderTextureBuffer->Shutdown();
		delete _renderTextureBuffer;
		_renderTextureBuffer = 0;
	}

	// Release the models object.
	for (auto object : _objects)
	{
		object->Shutdown();
		delete object;
		object = 0;
	}
	_objects.clear();

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

bool SceneShadows::Update(DX11Instance * direct3D, Input * input, ShaderManager * shaderManager, float frameTime)
{
	// Do the frame input processing.
	ProcessInput(input, frameTime);

	static float rotation = 0.0f;
	rotation += (float)XM_PI * 0.01f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	_cube->GetTransform()->SetRotation(0, rotation, 0);

	// Render the graphics.
	bool result = Draw(direct3D, shaderManager);
	if (!result)
	{
		return false;
	}

	return true;
}

void SceneShadows::ProcessInput(Input * input, float frameTime)
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

bool SceneShadows::Draw(DX11Instance* direct3D, ShaderManager* shaderManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;
	XMFLOAT3 objectPosition, objectRotation;

	// Render the scene to the render buffers.
	result = RenderSceneToTexture(direct3D, shaderManager);
	if (!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the View matrix based on the camera's Position.
	_camera->Render();

	// Generate the light view matrix based on the light's position.
	_light->GenerateViewMatrix();

	// Get the World, View, and Projection matrices from the camera and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	_camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);

	// Get the light's view and projection matrices from the light object.
	_light->GetViewMatrix(lightViewMatrix);
	_light->GetProjectionMatrix(lightProjectionMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	direct3D->TurnZBufferOff();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	direct3D->TurnZBufferOn();

	// Render all objects
	for (auto object : _objects)
	{
		direct3D->GetWorldMatrix(worldMatrix);

		objectPosition = object->GetTransform()->GetPositionValue();
		objectRotation = object->GetTransform()->GetRotationValue();

		// Rotate the world matrix by the rotation value so that the cube will spin.
		worldMatrix = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z) * 
			XMMatrixRotationX(objectRotation.x) *  XMMatrixRotationY(objectRotation.y) * XMMatrixRotationZ(objectRotation.z);

		// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		object->Render(direct3D->GetDeviceContext());

		// Render the model using the shadow shader.
		shaderManager->RenderShadowShader(direct3D->GetDeviceContext(), object->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
			lightProjectionMatrix, _textureManager->GetTexture(object->GetTextureIndex()), _renderTextureBuffer->GetShaderResourceView(0), _light->GetTransform()->GetPositionValue(),
			_light->GetAmbientColor(), _light->GetDiffuseColor());
	}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}

bool SceneShadows::RenderSceneToTexture(DX11Instance* direct3D, ShaderManager* shaderManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix, lightViewMatrix, lightProjectionMatrix;
	XMFLOAT3 objectPostion, objectRotation;

	// Set the render buffers to be the render target.
	_renderTextureBuffer->SetRenderTargets(direct3D->GetDeviceContext());

	// Clear the render buffers.
	_renderTextureBuffer->ClearRenderTargets(direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	_light->GenerateViewMatrix();

	// Get the World, View, and Projection matrices from the camera and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	_camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);

	// Get the view and orthographic matrices from the light object.
	_light->GetViewMatrix(lightViewMatrix);
	_light->GetProjectionMatrix(lightProjectionMatrix);

	// Render all objects
	for (auto object : _objects)
	{
		direct3D->GetWorldMatrix(worldMatrix);

		objectPostion = object->GetTransform()->GetPositionValue();
		objectRotation = object->GetTransform()->GetRotationValue();

		// Rotate the world matrix by the rotation value so that the cube will spin.
		worldMatrix = XMMatrixTranslation(objectPostion.x, objectPostion.y, objectPostion.z) *
			XMMatrixRotationX(objectRotation.x) *  XMMatrixRotationY(objectRotation.y) * XMMatrixRotationZ(objectRotation.z);

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		object->Render(direct3D->GetDeviceContext());

		// Render the model using the deferred shader.
		shaderManager->RenderDepthShader(direct3D->GetDeviceContext(), object->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	}

	// Reset the render target back to the original back buffer and not the render buffers anymore.
	direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	direct3D->ResetViewport();

	return true;
}