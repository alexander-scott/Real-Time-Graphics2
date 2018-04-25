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

	return true;
}

void SceneGraphics::Destroy()
{
	// Release the TargaTexture manager object.
	if (_textureManager)
	{
		_textureManager->Destroy();
		delete _textureManager;
		_textureManager = 0;
	}

	// Release the frustum object.
	if (_frustum)
	{
		delete _frustum;
		_frustum = 0;
	}

	// Release the camera object.
	if (_camera)
	{
		delete _camera;
		_camera = 0;
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

void SceneGraphics::ProcessInput(Input *, float)
{
	return;
}

bool SceneGraphics::Draw(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;
	XMFLOAT3 cameraPosition, skeletonPosition;

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

	// Construct the frustum.
	_frustum->ConstructFrustum(projectionMatrix, viewMatrix);

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off back face culling and turn off the Z buffer.
	direct3D->TurnOffCulling();
	direct3D->TurnZBufferOff();

	// Reset the world matrix.
	direct3D->GetWorldMatrix(worldMatrix);

	// Turn the Z buffer back and back face culling on.
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}
