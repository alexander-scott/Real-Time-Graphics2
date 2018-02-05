#include "Scene.h"

Scene::Scene()
{
	_camera = nullptr;
	_terrain = nullptr;
	_light = nullptr;
	_frustum = nullptr;
	_skyDome = nullptr;
}

Scene::Scene(const Scene& other)
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
{
	bool result;

	// Create the camera object.
	_camera = new Camera;
	if(!_camera)
	{
		return false;
	}

	// Set the initial Position of the camera and build the matrices needed for rendering.
	_camera->Render();
	_camera->RenderBaseViewMatrix();

	// Set the initial Position and rotation.
	_camera->GetTransform()->SetPosition(128.0f, 10.0f, -10.0f);
	_camera->GetTransform()->SetRotation(0.0f, 0.0f, 0.0f);

	// Create the light object.
	_light = new Light;
	if (!_light)
	{
		return false;
	}

	// Initialize the light object.
	_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	_light->SetDirection(-0.5f, -1.0f, -0.5f);

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

	// Create the terrain object.
	_terrain = new Terrain;
	if(!_terrain)
	{
		return false;
	}

	// Initialize the terrain object.
	result = _terrain->Initialize(Direct3D->GetDevice(), "setup.txt");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}
	
	// Set wire frame rendering initially to disabled.
	_wireFrame = false;

	// Set the rendering of cell lines initially to enabled.
	_cellLines = true;

	// Set the user locked to the terrain height for movement.
	_heightLocked = true;

	return true;
}

void Scene::Destroy()
{
	// Release the terrain object.
	if(_terrain)
	{
		_terrain->Destroy();
		delete _terrain;
		_terrain = 0;
	}

	// Release the sky dome object.
	if (_skyDome)
	{
		_skyDome->Destroy();
		delete _skyDome;
		_skyDome = 0;
	}

	// Release the frustum object.
	if (_frustum)
	{
		delete _frustum;
		_frustum = 0;
	}

	// Release the light object.
	if (_light)
	{
		delete _light;
		_light = 0;
	}

	// Release the camera object.
	if(_camera)
	{
		delete _camera;
		_camera = 0;
	}

	return;
}

bool Scene::Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, TextureManager* textureManager, float frameTime)
{
	bool result, foundHeight;
	float posX, posY, posZ, rotX, rotY, rotZ, height;

	// Do the frame input processing.
	HandleMovementInput(input, frameTime);

	// Get the View point Position/rotation.
	_camera->GetTransform()->GetPosition(posX, posY, posZ);
	_camera->GetTransform()->GetRotation(rotX, rotY, rotZ);

	// Do the terrain frame processing.
	_terrain->Update();

	// If the height is locked to the terrain then Position the camera on top of it.
	if (_heightLocked)
	{
		// Get the height of the triangle that is directly underneath the given camera Position.
		foundHeight = _terrain->GetHeightAtPosition(posX, posZ, height);
		if (foundHeight)
		{
			// If there was a triangle under the camera then Position the camera just above it By one meter.
			_camera->GetTransform()->SetPosition(posX, height + 1.0f, posZ);
		}
	}

	// Render the graphics.
	result = Render(direct3D, shaderManager, textureManager);
	if(!result)
	{
		return false;
	}

	return true;
}

void Scene::HandleMovementInput(Input* Input, float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Set the frame time for calculating the updated Position.
	_camera->GetTransform()->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = Input->IsLeftPressed();
	_camera->GetTransform()->TurnLeft(keyDown);

	keyDown = Input->IsRightPressed();
	_camera->GetTransform()->TurnRight(keyDown);

	keyDown = Input->IsUpPressed();
	_camera->GetTransform()->MoveForward(keyDown);

	keyDown = Input->IsDownPressed();
	_camera->GetTransform()->MoveBackward(keyDown);

	keyDown = Input->IsAPressed();
	_camera->GetTransform()->MoveUpward(keyDown);

	keyDown = Input->IsZPressed();
	_camera->GetTransform()->MoveDownward(keyDown);

	keyDown = Input->IsPgUpPressed();
	_camera->GetTransform()->LookUpward(keyDown);

	keyDown = Input->IsPgDownPressed();
	_camera->GetTransform()->LookDownward(keyDown);

	// Get the View point Position/rotation.
	_camera->GetTransform()->GetPosition(posX, posY, posZ);
	_camera->GetTransform()->GetRotation(rotX, rotY, rotZ);

	// Set the Position of the camera.
	_camera->GetTransform()->SetPosition(posX, posY, posZ);
	_camera->GetTransform()->SetRotation(rotX, rotY, rotZ);

	// Determine if the terrain should be rendered in wireframe or not.
	if (Input->IsF1Toggled())
	{
		_wireFrame = !_wireFrame;
	}

	// Determine if we should render the lines around each terrain cell.
	if (Input->IsF2Toggled())
	{
		_cellLines = !_cellLines;
	}

	// Determine if we should be locked to the terrain height when we move around or not.
	if (Input->IsF3Toggled())
	{
		_heightLocked = !_heightLocked;
	}

	return;
}

bool Scene::Render(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;
	XMFLOAT3 cameraPosition;

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

	// Reset the world matrix.
	direct3D->GetWorldMatrix(worldMatrix);

	// Turn the Z buffer back and back face culling on.
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	// Turn on wire frame rendering of the terrain if needed.
	if (_wireFrame)
	{
		direct3D->EnableWireframe();
	}

	// Render the terrain cells (and cell lines if needed).
	for (int i = 0; i<_terrain->GetCellCount(); i++)
	{
		// Render each terrain cell if it is visible only.
		result = _terrain->RenderCell(direct3D->GetDeviceContext(), i, _frustum);
		if (result)
		{
			// Render the cell buffers using the terrain shader.
			result = shaderManager->RenderTerrainShader(direct3D->GetDeviceContext(), _terrain->GetCellIndexCount(i), worldMatrix, viewMatrix,
				projectionMatrix, textureManager->GetTexture(0), textureManager->GetTexture(1), textureManager->GetTexture(2), textureManager->GetTexture(3),
				_light->GetTransform()->GetRotationValue(), _light->GetDiffuseColor());
			if (!result)
			{
				return false;
			}

			// If needed then render the bounding box around this terrain cell using the Colour shader. 
			if (_cellLines)
			{
				_terrain->RenderCellLines(direct3D->GetDeviceContext(), i);
				shaderManager->RenderColourShader(direct3D->GetDeviceContext(), _terrain->GetCellLinesIndexCount(i), worldMatrix,
					viewMatrix, projectionMatrix);
				if (!result)
				{
					return false;
				}
			}
		}
	}

	// Turn off wire frame rendering of the terrain if it was on.
	if (_wireFrame)
	{
		direct3D->DisableWireframe();
	}

	// Present the rendered scene to the screen.
	direct3D->EndScene();

	return true;
}