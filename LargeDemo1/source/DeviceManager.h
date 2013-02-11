#pragma once


class DeviceManager
{
public:
	static DeviceManager& Get()
	{ static DeviceManager inst; return inst; }

	bool InitDevice(HWND windowHandle, const DXGI_SAMPLE_DESC& multisamplingSettings, D3D11_CREATE_DEVICE_FLAG deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED);
	void CleanupDevice();

	void ClearBackAndDepthBuffer(const SimpleMath::Color& color = SimpleMath::Color(0.0f, 0.125f, 0.3f, 1.0f));

	IDXGISwapChain* GetSwapChain()			{ return _swapChain; }
	ID3D11Device* GetDevice()				{ return _device; }
	ID3D11DeviceContext* GetImmediateContext() { return _immediateContext; }

	unsigned int GetBackBufferWidth() { return _backBufferWidth; }
	unsigned int GetBackBufferHeight() { return _backBufferHeight; }

private:
	DeviceManager();
	~DeviceManager();

	bool _initialized;

	IDXGISwapChain* _swapChain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _immediateContext;

	D3D_DRIVER_TYPE _driverType;
	D3D_FEATURE_LEVEL _featureLevel;

	unsigned int _backBufferWidth, _backBufferHeight;
	ID3D11Texture2D* _backBuffer;
	ID3D11RenderTargetView* _backBufferView;

	ID3D11Texture2D* _depthBuffer;
	ID3D11DepthStencilView* _depthBufferView;

	D3D11_VIEWPORT _backBufferViewport;
};