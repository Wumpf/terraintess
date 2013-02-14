#pragma once

#include "Shader.h"

class DeviceManager
{
public:
	static DeviceManager& Get()
	{ static DeviceManager inst; return inst; }

	/// checks which sampling settings are available for a given format
	std::vector<DXGI_SAMPLE_DESC> GetAvailableMultisamplingSettings(DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	bool InitDevice(D3D11_CREATE_DEVICE_FLAG deviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED);
	bool CreateSwapChainAndBackBuffer(HWND windowHandle, const DXGI_SAMPLE_DESC& samplingSettings);
	void CleanupDevice();

	void ClearBackAndDepthBuffer(const SimpleMath::Color& color = SimpleMath::Color(0.0f, 0.125f, 0.3f, 1.0f));

	// easy states
	void SetRasterizerState(class RasterizerState& state);	// not const, since a state object may be created
	void SetSamplerState(class SamplerState& state, Shader::Type shaderType, unsigned int slot = 0);	// not const, since a state object may be created
	void SetSamplerStates(SamplerState* states, unsigned int numStates, Shader::Type shaderType, unsigned int startSlot = 0);

	// getter
	IDXGISwapChain1* GetSwapChain()			{ return _swapChain; }
	ID3D11Device* GetDevice()				{ return _device; }
	ID3D11DeviceContext* GetImmediateContext() { return _immediateContext; }

	unsigned int GetBackBufferWidth() const		{ return _backBufferWidth; }
	unsigned int GetBackBufferHeight() const	{ return _backBufferHeight; }
	float GetBackBufferAspectRatio() const		{ return static_cast<float>(_backBufferWidth) / _backBufferHeight; }

private:
	DeviceManager();
	~DeviceManager();

	bool _initialized;

	IDXGISwapChain1* _swapChain;
	ID3D11Device* _device;
	ID3D11DeviceContext* _immediateContext;

	D3D_DRIVER_TYPE _driverType;
	D3D_FEATURE_LEVEL _featureLevel;


	unsigned int _backBufferWidth, _backBufferHeight;
	ID3D11Texture2D* _backBuffer;
	ID3D11RenderTargetView* _backBufferView;

	ID3D11Texture2D* _depthBuffer;
	ID3D11DepthStencilView* _depthBufferView;

	DXGI_SAMPLE_DESC _samplingSettings;

	D3D11_VIEWPORT _backBufferViewport;
};