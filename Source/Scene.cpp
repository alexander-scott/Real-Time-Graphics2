#include "Scene.h"

Scene::Scene()
{
	_camera = nullptr;
	_terrain = nullptr;
	_light = nullptr;
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
	bool result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Do the frame input processing.
	HandleMovementInput(input, frameTime);

	// Get the View point Position/rotation.
	_camera->GetTransform()->GetPosition(posX, posY, posZ);
	_camera->GetTransform()->GetRotation(rotX, rotY, rotZ);

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

	if(Input->IsF1Toggled())
	{
		// No action to map to yet
	}

	// Determine if the terrain should be rendered in wireframe or not.
	if (Input->IsF2Toggled())
	{
		_wireFrame = !_wireFrame;
	}

	return;
}

bool Scene::Render(DX11Instance* direct3D, ShaderManager* shaderManager, TextureManager* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	// Generate the View matrix based on the camera's Position.
	_camera->Render();

	// Get the World, View, and Projection matrices from the camera and d3d objects.
	direct3D->GetWorldMatrix(worldMatrix);
	_camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	_camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);
	
	// Clear the buffers to begin the scene.
	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Turn on wire frame rendering of the terrain if needed.
	if (_wireFrame)
	{
		direct3D->EnableWireframe();
	}

	// Render the terrain grid using the light shader.
	_terrain->Render(direct3D->GetDeviceContext());

	//result = shaderManager->RenderTextureShader(direct3D->GetDeviceContext(), _terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, textureManager->GetTexture(1));
	/*result = shaderManager->RenderLightShader(direct3D->GetDeviceContext(), _terrain->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, textureManager->GetTexture(1), _light->GetDirection(), _light->GetDiffuseColor());*/
	result = shaderManager->RenderTerrainShader(direct3D->GetDeviceContext(), _terrain->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, textureManager->GetTexture(0), _light->GetDirection(), _light->GetDiffuseColor());
	
	if(!result)
	{
		return false;
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