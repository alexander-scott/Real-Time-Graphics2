#include "Scene.h"

Scene::Scene()
{
	m_Camera = 0;
	m_Transform = 0;
	m_Terrain = 0;
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
	m_Camera = new Camera;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera and build the matrices needed for rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	// Create the position object.
	m_Transform = new Transform;
	if(!m_Transform)
	{
		return false;
	}

	// Set the initial position and rotation.
	m_Transform->SetPosition(128.0f, 10.0f, -10.0f);
	m_Transform->SetRotation(0.0f, 0.0f, 0.0f);

	// Create the terrain object.
	m_Terrain = new Terrain;
	if(!m_Terrain)
	{
		return false;
	}

	// Initialize the terrain object.
	result = m_Terrain->Initialize(Direct3D->GetDevice(), "Source/setup.txt");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}
	
	// Set wire frame rendering initially to enabled.
	m_wireFrame = true;

	return true;
}

void Scene::Destroy()
{
	// Release the terrain object.
	if(m_Terrain)
	{
		m_Terrain->Destroy();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the position object.
	if(m_Transform)
	{
		delete m_Transform;
		m_Transform = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	return;
}

bool Scene::Update(DX11Instance* Direct3D, Input* Input, ShaderManager* ShaderManager, float frameTime)
{
	bool result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Do the frame input processing.
	HandleMovementInput(Input, frameTime);

	// Get the view point position/rotation.
	m_Transform->GetPosition(posX, posY, posZ);
	m_Transform->GetRotation(rotX, rotY, rotZ);

	// Render the graphics.
	result = Render(Direct3D, ShaderManager);
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

	// Set the frame time for calculating the updated position.
	m_Transform->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = Input->IsLeftPressed();
	m_Transform->TurnLeft(keyDown);

	keyDown = Input->IsRightPressed();
	m_Transform->TurnRight(keyDown);

	keyDown = Input->IsUpPressed();
	m_Transform->MoveForward(keyDown);

	keyDown = Input->IsDownPressed();
	m_Transform->MoveBackward(keyDown);

	keyDown = Input->IsAPressed();
	m_Transform->MoveUpward(keyDown);

	keyDown = Input->IsZPressed();
	m_Transform->MoveDownward(keyDown);

	keyDown = Input->IsPgUpPressed();
	m_Transform->LookUpward(keyDown);

	keyDown = Input->IsPgDownPressed();
	m_Transform->LookDownward(keyDown);

	// Get the view point position/rotation.
	m_Transform->GetPosition(posX, posY, posZ);
	m_Transform->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	if(Input->IsF1Toggled())
	{
		// No action to map to yet
	}

	// Determine if the terrain should be rendered in wireframe or not.
	if (Input->IsF2Toggled())
	{
		m_wireFrame = !m_wireFrame;
	}

	return;
}

bool Scene::Render(DX11Instance* Direct3D, ShaderManager* ShaderManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	Direct3D->GetOrthoMatrix(orthoMatrix);
	
	// Clear the buffers to begin the scene.
	Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Turn on wire frame rendering of the terrain if needed.
	if (m_wireFrame)
	{
		Direct3D->EnableWireframe();
	}

	// Render the terrain grid using the color shader.
	m_Terrain->Render(Direct3D->GetDeviceContext());
	result = ShaderManager->RenderColorShader(Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, 
											  projectionMatrix);
	if(!result)
	{
		return false;
	}

	// Turn off wire frame rendering of the terrain if it was on.
	if (m_wireFrame)
	{
		Direct3D->DisableWireframe();
	}

	// Present the rendered scene to the screen.
	Direct3D->EndScene();

	return true;
}