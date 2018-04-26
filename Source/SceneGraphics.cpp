#include "SceneGraphics.h"

SceneGraphics::SceneGraphics()
{
}

SceneGraphics::SceneGraphics(const SceneGraphics &)
{
}

bool SceneGraphics::Initialize(DX11Instance * Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
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

	result = _textureManager->LoadJPEGTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), L"Source/terrain/Crate_COLOR.dds", 42);
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

	// Set the initial Position of the camera and build the matrices needed for rendering.
	_camera->Render();
	_camera->RenderBaseViewMatrix();

	// Set the initial Position and rotation.
	_camera->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	_camera->GetTransform()->SetRotation(0.0f, 0.0f, 0.0f);

	// Create the frustum object.
	_frustum = new Frustum;
	if (!_frustum)
	{
		return false;
	}

	// Initialize the frustum object.
	_frustum->Initialize(screenDepth);

	_cube = new NewCube;
	if (!_cube)
	{
		return false;
	}

	// Initalise the cube object
	_cube->Initialize(Direct3D->GetDevice());
	_cube->GetTransform()->SetPosition(XMFLOAT3(0, 0, 10));

	return true;
}

void SceneGraphics::Destroy()
{
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

	// Release the cube object.
	if (_cube)
	{
		_cube->Destroy();
		delete _cube;
		_cube = nullptr;
	}

	return;
}

bool SceneGraphics::Update(DX11Instance * direct3D, Input * input, ShaderManager * shaderManager, float frameTime)
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

void SceneGraphics::ProcessInput(Input * input, float frameTime)
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

bool SceneGraphics::Draw(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;
	XMFLOAT3 cameraPosition, cubePosition;

	// Generate the View matrix based on the camera's Position.
	_camera->Render();

	// Get the World, View, and Projection matrices from the camera and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	_camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);

	// Get the Position of the camera.
	_camera->GetTransform()->GetPosition(cameraPosition);

	// Get the position of the cube
	_cube->GetTransform()->GetPosition(cubePosition);

	// Construct the frustum.
	_frustum->ConstructFrustum(projectionMatrix, viewMatrix);

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off back face culling and turn off the Z buffer.
	direct3D->TurnOffCulling();
	direct3D->TurnZBufferOff();

	// Reset the world matrix.
	direct3D->GetWorldMatrix(worldMatrix);

	SurfaceInfo surface;
	surface.AmbientMtrl = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	surface.DiffuseMtrl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	surface.SpecularMtrl = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	LightStruct light;
	light.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	light.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	light.SpecularPower = 20.0f;
	light.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	worldMatrix = XMMatrixTranslation(cubePosition.x, cubePosition.y, cubePosition.z);

	_cube->Draw(direct3D->GetDeviceContext());
	result = shaderManager->RenderLightShader(direct3D->GetDeviceContext(), _cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
		_textureManager->GetTexture(42), light.LightVecW, light.DiffuseLight);
	/*result = shaderManager->RenderCubeShader(direct3D->GetDeviceContext(), _cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		surface, light, light.LightVecW, true, _textureManager->GetTexture(42));*/
	//result = shaderManager->RenderColourShader(direct3D->GetDeviceContext(), _cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back and back face culling on.
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}
