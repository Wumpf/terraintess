#include "stdafx.h"
#include "DeviceManager.h"
#include "Utils.h"

#include "RasterizerState.h"
#include "SamplerState.h"
#include "BlendState.h"

DeviceManager::DeviceManager() :
	_initialized(false),
	_device(nullptr),

	_backBuffer(nullptr),
	_backBufferView(nullptr),
	_depthBuffer(nullptr),
	_depthBufferView(nullptr),
	_swapChain(nullptr)
{
}


DeviceManager::~DeviceManager(void)
{
	CleanupDevice();
}


std::vector<DXGI_SAMPLE_DESC> DeviceManager::GetAvailableMultisamplingSettings(DXGI_FORMAT format)
{
	assert(_device);

	std::vector<DXGI_SAMPLE_DESC> out;
	DXGI_SAMPLE_DESC noMultisampling = { 1, 0 }; 
	out.push_back(noMultisampling);

	DXGI_SAMPLE_DESC currentDesc;
	for(currentDesc.Count = 1; currentDesc.Count < D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ++currentDesc.Count)
	{
		UINT numQualityLevels;
		if(SUCCEEDED(_device->CheckMultisampleQualityLevels(format, currentDesc.Count, &numQualityLevels)) && numQualityLevels > 0)
		{
			for(currentDesc.Quality = 0; currentDesc.Quality < numQualityLevels; ++currentDesc.Quality)
				out.push_back(currentDesc);
		}
	}

	return out;
}

bool DeviceManager::InitDevice(D3D11_CREATE_DEVICE_FLAG deviceFlags)
{
	if(_initialized)
		return true;
	HRESULT hr = S_OK;


	// device creation flags
    UINT createDeviceFlags = deviceFlags;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;//  | D3D11_CREATE_DEVICE_DEBUGGABLE;
#endif

	// driver types by priority
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

	// feature levels by priority
    D3D_FEATURE_LEVEL featureLevels[] =
    {
		D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice(NULL, _driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, 
										&_device, &_featureLevel, &_immediateContext);
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return false;

	_initialized = true;

	return true;
}

bool DeviceManager::CreateSwapChainAndBackBuffer(HWND windowHandle, const DXGI_SAMPLE_DESC& multisamplingSettings)
{
	_samplingSettings = multisamplingSettings;
	HRESULT hr;

	Utils::SafeReleaseCOM(_backBuffer);
	Utils::SafeReleaseCOM(_backBufferView);
	Utils::SafeReleaseCOM(_depthBuffer);
	Utils::SafeReleaseCOM(_depthBufferView);
	Utils::SafeReleaseCOM(_swapChain);

	// width from window
    RECT rc;
    GetClientRect(windowHandle, &rc);
    _backBufferWidth = rc.right - rc.left;
    _backBufferHeight = rc.bottom - rc.top;

	// query dxgi factory
	CComPtr<IDXGIDevice2> dxgiDevice;
	_device->QueryInterface(__uuidof(IDXGIDevice2), (void **)&dxgiDevice);
	dxgiDevice->SetMaximumFrameLatency(1);
	CComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);
	CComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

	// create swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    sd.BufferCount = 1;
    sd.Width = _backBufferWidth;
    sd.Height = _backBufferHeight;
    sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	
	sd.Stereo = false;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Scaling = DXGI_SCALING_NONE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.Flags = 0;
	sd.SampleDesc = multisamplingSettings;
	hr = dxgiFactory->CreateSwapChainForHwnd(_device, windowHandle, &sd, nullptr, nullptr, &_swapChain);
	if(FAILED(hr))
		return false;

	// create backbuffer & view
	hr = _swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&_backBuffer);
	if(FAILED(hr))
		return false;
	hr = _device->CreateRenderTargetView( _backBuffer, NULL, &_backBufferView);
	if(FAILED(hr))
		return false;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = _backBufferWidth;
    descDepth.Height = _backBufferHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc = multisamplingSettings;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = _device->CreateTexture2D( &descDepth, NULL, &_depthBuffer);
	if(FAILED(hr))
		return false;
    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = multisamplingSettings.Count > 0 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
	hr = _device->CreateDepthStencilView( _depthBuffer, &descDSV, &_depthBufferView);
	if(FAILED(hr))
		return false;

	_immediateContext->OMSetRenderTargets( 1, &_backBufferView, _depthBufferView);

    // Setup the viewport
    _backBufferViewport.Width = (FLOAT)_backBufferWidth;
    _backBufferViewport.Height = (FLOAT)_backBufferHeight;
    _backBufferViewport.MinDepth = 0.0f;
    _backBufferViewport.MaxDepth = 1.0f;
    _backBufferViewport.TopLeftX = 0;
    _backBufferViewport.TopLeftY = 0;
    _immediateContext->RSSetViewports( 1, &_backBufferViewport );

	return true;
}

void DeviceManager::ClearBackAndDepthBuffer(const SimpleMath::Color& color)
{
	_immediateContext->ClearRenderTargetView(_backBufferView, color);
	_immediateContext->ClearDepthStencilView(_depthBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DeviceManager::SetRasterizerState(RasterizerState& state)
{
	if(state._stateObject == nullptr)
		_device->CreateRasterizerState(&state._desc, &state._stateObject.p);

	_immediateContext->RSSetState(state._stateObject.p);
}

void DeviceManager::SetBlendState(class BlendState& state, const SimpleMath::Vector4 blendFactor, UINT sampleMask)
{
	if(state._stateObject == nullptr)
		_device->CreateBlendState(&state._desc, &state._stateObject.p);

	_immediateContext->OMSetBlendState(state._stateObject.p, blendFactor, sampleMask);
}

void DeviceManager::SetSamplerState(class SamplerState& state, Shader::Type shaderType, unsigned int slot)
{
	if(state._stateObject == nullptr)
		_device->CreateSamplerState(&state._desc, &state._stateObject.p);

	switch(shaderType)
	{
	case Shader::Type::PIXEL:
		_immediateContext->PSSetSamplers(slot, 1, &state._stateObject.p);
		break;
	case Shader::Type::VERTEX:
		_immediateContext->VSSetSamplers(slot, 1, &state._stateObject.p);
		break;
	case Shader::Type::GEOMETRY:
		_immediateContext->GSSetSamplers(slot, 1, &state._stateObject.p);
		break;
	case Shader::Type::HULL:
		_immediateContext->HSSetSamplers(slot, 1, &state._stateObject.p);
		break;
	case Shader::Type::DOMAIN:
		_immediateContext->DSSetSamplers(slot, 1, &state._stateObject.p);
		break;
	case Shader::Type::COMPUTE:
		_immediateContext->CSSetSamplers(slot, 1, &state._stateObject.p);
		break;
	}
}


void DeviceManager::SetSamplerState(class SamplerState& state, unsigned int slot)
{
	if(state._stateObject == nullptr)
		_device->CreateSamplerState(&state._desc, &state._stateObject.p);

	_immediateContext->PSSetSamplers(slot, 1, &state._stateObject.p);
	_immediateContext->VSSetSamplers(slot, 1, &state._stateObject.p);
	_immediateContext->GSSetSamplers(slot, 1, &state._stateObject.p);
	_immediateContext->HSSetSamplers(slot, 1, &state._stateObject.p);
	_immediateContext->DSSetSamplers(slot, 1, &state._stateObject.p);
	_immediateContext->CSSetSamplers(slot, 1, &state._stateObject.p);
}

void DeviceManager::CleanupDevice()
{
	if(!_initialized)
		return;


	Utils::SafeReleaseCOM(_backBuffer);
	Utils::SafeReleaseCOM(_backBufferView);

	Utils::SafeReleaseCOM(_depthBuffer);
	Utils::SafeReleaseCOM(_depthBufferView);

	Utils::SafeReleaseCOM(_swapChain);
	Utils::SafeReleaseCOM(_device);
	Utils::SafeReleaseCOM(_immediateContext);


	_initialized = false;
}