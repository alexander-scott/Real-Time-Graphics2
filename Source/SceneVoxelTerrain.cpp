#include "SceneVoxelTerrain.h"

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

	// Create the sky dome object.
	_skyDome = new SkyDome;
	if (!_skyDome)
	{
		return false;
	}

	// Initialize the sky dome object.
	result = _skyDome->Initialize(Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky dome object.", L"Error", MB_OK);
		return false;
	}

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

	// Create terrain
	for (int x = 0; x < TERRAIN_SIZE; x++)
	{
		for (int y = 0; y < TERRAIN_SIZE; y++)
		{
			for (int z = 0; z < TERRAIN_SIZE; z++)
			{
				_terrain[x][y][z] = 1;
			}
		}
	}

	return true;
}

void SceneVoxelTerrain::Destroy()
{
	// Release the deferred buffers object.
	if (_renderTextureBuffer)
	{
		_renderTextureBuffer->Shutdown();
		delete _renderTextureBuffer;
		_renderTextureBuffer = 0;
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

	// Release the sky dome object.
	if (_skyDome)
	{
		_skyDome->Destroy();
		delete _skyDome;
		_skyDome = 0;
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
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;
	XMFLOAT3 objectPosition, objectRotation, cameraPosition;

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

	// Get the Position of the camera.
	_camera->GetTransform()->GetPosition(cameraPosition);

	// Get the light's view and projection matrices from the light object.
	_light->GetViewMatrix(lightViewMatrix);
	_light->GetProjectionMatrix(lightProjectionMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	direct3D->TurnZBufferOff();
	direct3D->TurnOffCulling();

	// Translate the sky dome to be centered around the camera Position.
	worldMatrix = XMMatrixTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Render the sky dome using the sky dome shader.
	_skyDome->Draw(direct3D->GetDeviceContext());
	result = shaderManager->RenderSkyDomeShader(direct3D->GetDeviceContext(), _skyDome->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, _skyDome->GetApexColor(), _skyDome->GetCenterColor());
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	_voxel->Render(direct3D->GetDeviceContext());

	for (int x = 0; x < TERRAIN_SIZE; x++)
	{
		for (int y = 0; y < TERRAIN_SIZE; y++)
		{
			for (int z = 0; z < TERRAIN_SIZE; z++)
			{
				if (_terrain[x][y][z] == 1)
				{
					direct3D->GetWorldMatrix(worldMatrix);
					worldMatrix *= XMMatrixTranslation(x, y, z);

					// Render the model using the shadow shader.
					shaderManager->RenderShadowShader(direct3D->GetDeviceContext(), _voxel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
						lightProjectionMatrix, _textureManager->GetTexture(_voxel->GetTextureIndex()), _renderTextureBuffer->GetShaderResourceView(0), _light->GetTransform()->GetPositionValue(),
						_light->GetAmbientColor(), _light->GetDiffuseColor());
				}
			}
		}
	}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}

bool SceneVoxelTerrain::RenderSceneToTexture(DX11Instance* direct3D, ShaderManager* shaderManager)
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

	_voxel->Render(direct3D->GetDeviceContext());

	for (int x = 0; x < TERRAIN_SIZE; x++)
	{
		for (int y = 0; y < TERRAIN_SIZE; y++)
		{
			for (int z = 0; z < TERRAIN_SIZE; z++)
			{
				if (_terrain[x][y][z] == 1)
				{
					direct3D->GetWorldMatrix(worldMatrix);
					worldMatrix *= XMMatrixTranslation(x, y, z);

					shaderManager->RenderDepthShader(direct3D->GetDeviceContext(), _voxel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
				}
			}
		}
	}

	// Reset the render target back to the original back buffer and not the render buffers anymore.
	direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	direct3D->ResetViewport();

	return true;
}