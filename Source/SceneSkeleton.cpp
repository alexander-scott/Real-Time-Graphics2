#include "SceneSkeleton.h"

SceneSkeleton::SceneSkeleton() : IScene()
{
}

SceneSkeleton::~SceneSkeleton()
{
}

bool SceneSkeleton::Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
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

	// Initialize the skeleton object.
	_skeleton = new Skeleton;
	if (!_skeleton)
	{
		return false;
	}

	// Initalize the skeleton object.
	_skeleton->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), _textureManager, L"Source/Animation/boy.md5mesh", L"Source/Animation/boy.md5anim");
	_skeleton->GetTransform()->SetPosition(XMFLOAT3(0, 0, 100));

	return true;
}

void SceneSkeleton::Destroy()
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

	// Release the skeleton object.
	if (_skeleton)
	{
		delete _skeleton;
		_skeleton = 0;
	}

	return;
}

bool SceneSkeleton::Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, float frameTime)
{
	// Do the frame input processing.
	ProcessInput(input, frameTime);

	_skeleton->Update(direct3D->GetDeviceContext(), frameTime);

	// Render the graphics.
	bool result = Draw(direct3D, shaderManager, _textureManager);
	if (!result)
	{
		return false;
	}

	return true;
}

void SceneSkeleton::ProcessInput(Input* input, float frameTime)
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

bool SceneSkeleton::Draw(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
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

	// Get the position of the skeleton
	_skeleton->GetTransform()->GetPosition(skeletonPosition);

	// Construct the frustum.
	_frustum->ConstructFrustum(projectionMatrix, viewMatrix);

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off back face culling and turn off the Z buffer.
	direct3D->TurnOffCulling();
	direct3D->TurnZBufferOff();

	// Turn the Z buffer back and back face culling on.
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	// Reset the world matrix.
	direct3D->GetWorldMatrix(worldMatrix);

	// Translate the sky dome to be centered around the skeleton Position.
	worldMatrix = XMMatrixTranslation(skeletonPosition.x, skeletonPosition.y, skeletonPosition.z);

	for (int i = 0; i < _skeleton->GetSubsetCount(); i++)
	{
		_skeleton->DrawSubset(direct3D->GetDeviceContext(), i);
		//result = shaderManager->RenderColourShader(direct3D->GetDeviceContext(), _skeleton->GetIndexCount(i), worldMatrix, viewMatrix, projectionMatrix);
		result = shaderManager->RenderTextureShader(direct3D->GetDeviceContext(), _skeleton->GetIndexCount(i), worldMatrix, viewMatrix, projectionMatrix, textureManager->GetTexture(10 + i));

		if (!result)
		{
			return false;
		}
	}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}
