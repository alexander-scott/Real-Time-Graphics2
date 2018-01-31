#include "Scene.h"

Scene::Scene()
{
	m_UserInterface = 0;
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

	// Create the user interface object.
	m_UserInterface = new UserInterface;
	if(!m_UserInterface)
	{
		return false;
	}

	// Initialize the user interface object.
	result = m_UserInterface->Initialize(Direct3D, screenHeight, screenWidth);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the user interface object.", L"Error", MB_OK);
		return false;
	}

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
	m_Transform->SetPosition(128.0f, 5.0f, -10.0f);
	m_Transform->SetRotation(0.0f, 0.0f, 0.0f);

	// Create the terrain object.
	m_Terrain = new Terrain;
	if(!m_Terrain)
	{
		return false;
	}

	// Initialize the terrain object.
	result = m_Terrain->Initialize(Direct3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}
	
	// Set the UI to display by default.
	m_displayUI = true;

	return true;
}

void Scene::Shutdown()
{
	// Release the terrain object.
	if(m_Terrain)
	{
		m_Terrain->Shutdown();
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

	// Release the user interface object.
	if(m_UserInterface)
	{
		m_UserInterface->Shutdown();
		delete m_UserInterface;
		m_UserInterface = 0;
	}

	return;
}

bool Scene::Update(DX11Instance* Direct3D, Input* Input, ShaderManager* ShaderManager, float frameTime, int fps)
{
	bool result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Do the frame input processing.
	HandleMovementInput(Input, frameTime);

	// Get the view point position/rotation.
	m_Transform->GetPosition(posX, posY, posZ);
	m_Transform->GetRotation(rotX, rotY, rotZ);

	// Do the frame processing for the user interface.
	result = m_UserInterface->Update(Direct3D->GetDeviceContext(), fps, posX, posY, posZ, rotX, rotY, rotZ);
	if(!result)
	{
		return false;
	}

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

	// Determine if the user interface should be displayed or not.
	if(Input->IsF1Toggled())
	{
		m_displayUI = !m_displayUI;
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

	// Render the terrain grid using the color shader.
	m_Terrain->Render(Direct3D->GetDeviceContext());
	result = ShaderManager->RenderColorShader(Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, 
											  projectionMatrix);
	if(!result)
	{
		return false;
	}

	// Render the user interface.
	if(m_displayUI)
	{
		result = m_UserInterface->Render(Direct3D, ShaderManager, worldMatrix, baseViewMatrix, orthoMatrix);
		if(!result)
		{
			return false;
		}
	}

	// Present the rendered scene to the screen.
	Direct3D->EndScene();

	return true;
}