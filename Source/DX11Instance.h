#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class DX11Instance
{
public:
	DX11Instance();
	DX11Instance(const DX11Instance&);
	~DX11Instance();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Destroy();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnCulling();
	void TurnOffCulling();

	void EnableAlphaBlending();
	void EnableAlphaToCoverageBlending();
	void DisableAlphaBlending();

	void EnableWireframe();
	void DisableWireframe();

	void SetBackBufferRenderTarget();
	void ResetViewport();

private:
	bool						_vsyncEnabled;
	IDXGISwapChain*				_swapChain;
	ID3D11Device*				_device;
	ID3D11DeviceContext*		_deviceContext;
	ID3D11RenderTargetView*		_renderTargetView;
	ID3D11Texture2D*			_depthStencilBuffer;
	ID3D11DepthStencilState*	_depthStencilState;
	ID3D11DepthStencilView*		_depthStencilView;
	ID3D11RasterizerState*		_rasterState;
	ID3D11RasterizerState*		_rasterStateNoCulling;
	ID3D11RasterizerState*		_rasterStateWireframe;
	XMMATRIX					_projectionMatrix;
	XMMATRIX					_worldMatrix;
	XMMATRIX					_orthoMatrix;
	ID3D11DepthStencilState*	_depthDisabledStencilState;
	ID3D11BlendState*			_alphaEnableBlendingState;
	ID3D11BlendState*			_alphaDisableBlendingState;
	ID3D11BlendState*			_alphaEnableBlendingState2;
	D3D11_VIEWPORT				_viewport;
};