#include "SceneVoxelTerrain.h"

#include "FastNoise.h"

SceneVoxelTerrain::SceneVoxelTerrain()
{
	_light = 0;
}

bool SceneVoxelTerrain::Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
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

	// Create the camera object.
	_camera = new Camera;
	if (!_camera)
	{
		return false;
	}

	// Set the initial Position and rotation.
	_camera->GetTransform()->SetPosition(0.0f, 0.0f, -200.0f);
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
	_voxel = new Object;
	if (!_voxel)
	{
		return false;
	}

	// Initialize the model object.
	result = _voxel->Initialize(Direct3D->GetDevice(), "Source/shadows/cube.txt", 47);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Initalize the terrain object.
	_voxelTerrain = new VoxelTerrain;

	result = _voxelTerrain->Initialize(Direct3D->GetDevice(), "Source/shadows/cube.txt", 47);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SceneVoxelTerrain::Destroy()
{
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

bool SceneVoxelTerrain::Update(DX11Instance * direct3D, Input * input, ShaderManager * shaderManager, float frameTime)
{
	// Do the frame input processing.
	ProcessInput(input, frameTime);

	// Render the graphics.
	bool result = Draw(direct3D, shaderManager);
	if (!result)
	{
		return false;
	}

	return true;
}

void SceneVoxelTerrain::ProcessInput(Input * input, float frameTime)
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

bool SceneVoxelTerrain::Draw(DX11Instance* direct3D, ShaderManager* shaderManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;
	XMFLOAT3 cameraPosition;

	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

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

	// Turn the Z buffer back on now that all 2D rendering has completed.
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	for (int z = 0; z < CHUNK_COUNT; z++)
	{
		for (int y = 0; y < CHUNK_COUNT; y++)
		{
			for (int x = 0; x < CHUNK_COUNT; x++)
			{
				if (_voxelTerrain->HasBlocks(x, y, z))
				{
					_voxelTerrain->Render(direct3D->GetDeviceContext(), x, y, z);
					shaderManager->RenderColourShader(direct3D->GetDeviceContext(), _voxelTerrain->GetIndexCount(x, y, z), worldMatrix, viewMatrix, projectionMatrix);
				}
			}
		}
	}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}