#pragma once


class Direct3D11Device
{
public:
	static Direct3D11Device& Get()
	{ static Direct3D11Device inst; return inst; }

	bool InitDevice(HWND windowHandle, const DXGI_SAMPLE_DESC& multisamplingSettings, D3D11_CREATE_DEVICE_FLAG deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED);
	void CleanupDevice();

	void ClearBackAndDepthBuffer(const DirectX::SimpleMath::Color& color = DirectX::SimpleMath::Color(0.0f, 0.125f, 0.3f, 1.0f));

	IDXGISwapChain* GetSwapChain()			{ return _swapChain; }
	ID3D11Device* GetDevice()				{ return _device; }
	ID3D11DeviceContext* GetImmediateContext() { return _immediateContext; }

private:
	Direct3D11Device();
	~Direct3D11Device();

	bool _initialized;

	IDXGISwapChain* _swapChain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _immediateContext;

	D3D_DRIVER_TYPE _driverType;
	D3D_FEATURE_LEVEL _featureLevel;


	ID3D11Texture2D* _backBuffer;
	ID3D11RenderTargetView* _backBufferView;

	ID3D11Texture2D* _depthBuffer;
	ID3D11DepthStencilView* _depthBufferView;

	D3D11_VIEWPORT _backBufferViewport;
};