#include "SceneCombined.h"

SceneCombined::SceneCombined() : IScene()
{
}

SceneCombined::SceneCombined(const SceneCombined& other)
{
}

bool SceneCombined::Initialize(DX11Instance* Direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
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

	result = _textureManager->LoadTargaTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), "Source/terrain/rock01d.tga", 0);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadTargaTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), "Source/terrain/rock01n.tga", 1);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadTargaTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), "Source/terrain/snow01n.tga", 2);
	if (!result)
	{
		return false;
	}

	result = _textureManager->LoadTargaTexture(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), "Source/terrain/distance01n.tga", 3);
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
	_camera->GetTransform()->SetPosition(128.0f, 20.0f, 500.0f);
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
	if (!_terrain)
	{
		return false;
	}

	// Initialize the terrain object.
	result = _terrain->Initialize(Direct3D->GetDevice(), "setup.txt");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the skeleton object.
	_skeleton = new Skeleton;
	if (!_skeleton)
	{
		return false;
	}

	// Initalize the skeleton object.
	_skeleton->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), _textureManager, L"Source/Animation/boy.md5mesh", L"Source/Animation/boy.md5anim");
	_skeleton->GetTransform()->SetPosition(XMFLOAT3(128.0f, 20.0f, 500.0f));
	_skeleton->GetTransform()->SetRotation(XMFLOAT3(0, 0, 0));
	_skeleton->GetTransform()->SetScale(XMFLOAT3(0.1f, 0.1f, 0.1f));

	// Set wire frame rendering initially to disabled.
	_wireFrame = false;

	// Set the rendering of cell lines initially to disabled.
	_cellLines = false;

	// Set the user locked to the terrain height for movement.
	_heightLocked = true;

	return true;
}

void SceneCombined::Destroy()
{
	// Release the TargaTexture manager object.
	if (_textureManager)
	{
		_textureManager->Destroy();
		delete _textureManager;
		_textureManager = 0;
	}

	// Release the terrain object.
	if (_terrain)
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

	// Release the skeleton object.
	if (_skeleton)
	{
		delete _skeleton;
		_skeleton = 0;
	}

	// Release the light object.
	if (_light)
	{
		delete _light;
		_light = 0;
	}

	// Release the camera object.
	if (_camera)
	{
		delete _camera;
		_camera = 0;
	}

	return;
}

bool SceneCombined::Update(DX11Instance* direct3D, Input* input, ShaderManager* shaderManager, float frameTime)
{
	bool result, foundHeight;
	float posX, posY, posZ, rotX, rotY, rotZ, height;

	// Do the frame input processing.
	ProcessInput(input, frameTime);

	_skeleton->Update(direct3D->GetDeviceContext(), frameTime);

	// Get the View point Position/rotation.
	_camera->GetTransform()->GetPosition(posX, posY, posZ);
	_camera->GetTransform()->GetRotation(rotX, rotY, rotZ);

	// Do the terrain frame processing.
	_terrain->Update();

	// If the height is locked to the terrain then Position the camera on top of it.
	if (_heightLocked)
	{
		XMFLOAT3 cameraRot;
		_camera->GetTransform()->GetRotation(cameraRot);

		float pitchRadian = cameraRot.x * (XM_PI / 180); // X rotation
		float yawRadian = cameraRot.y * (XM_PI / 180); // Y rotation

		float newPosX = -15 * (sinf(yawRadian) * cosf(pitchRadian));
		float newPosY = -15 * (-sinf(pitchRadian));
		float newPosZ = -15 * (cosf(yawRadian) * cosf(pitchRadian));

		// Get the height of the triangle that is directly underneath the given camera Position.
		foundHeight = _terrain->GetHeightAtPosition(posX - newPosX, posZ - newPosZ, height);
		if (foundHeight)
		{
			_camera->GetTransform()->SetPosition(posX, height + 6.0f, posZ);

			// If there was a triangle under the camera then Position the camera just above it
			_skeleton->GetTransform()->SetPosition(posX - newPosX, height + 3, posZ - newPosZ);
			_skeleton->GetTransform()->SetRotation(0, XM_PI + yawRadian, 0);
		}
	}

	// Render the graphics.
	result = Draw(direct3D, shaderManager, _textureManager);
	if (!result)
	{
		return false;
	}

	return true;
}

void SceneCombined::ProcessInput(Input* Input, float frameTime)
{
	bool keyDown;

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

bool SceneCombined::Draw(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;
	XMFLOAT3 cameraPosition;
	XMFLOAT3 skeletonPosition, skeletonRotation, skeletonScale;

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

	// Get the skeleton details
	_skeleton->GetTransform()->GetPosition(skeletonPosition);
	_skeleton->GetTransform()->GetRotation(skeletonRotation);
	_skeleton->GetTransform()->GetScale(skeletonScale);

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

	// Translate the sky dome to be centered around the skeleton Position.
	worldMatrix = XMMatrixScaling(skeletonScale.x, skeletonScale.y, skeletonScale.z) *
		(XMMatrixRotationX(skeletonRotation.x) * XMMatrixRotationY(skeletonRotation.y) * XMMatrixRotationZ(skeletonRotation.z)) *
		XMMatrixTranslation(skeletonPosition.x, skeletonPosition.y, skeletonPosition.z);

	for (int i = 0; i < _skeleton->GetSubsetCount(); i++)
	{
		_skeleton->DrawSubset(direct3D->GetDeviceContext(), i);
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