#include "stdafx.h"
#include "Demo.h"
#include "Window.h"
#include "Direct3D11Device.h"

Demo::Demo() :
	_window(new Window())
{
}

Demo::~Demo()
{
}

bool Demo::Initialize(HINSTANCE hInstance)
{
	if(!_window->InitWindow(hInstance, L"LargeDemo1", 1600, 900))
		return false;

	DXGI_SAMPLE_DESC multiSamplingSettings;
	multiSamplingSettings.Count = 1;
	multiSamplingSettings.Quality = 0;
	Direct3D11Device::Get().InitDevice(_window->GetHandle(), multiSamplingSettings);

	return true;
}

void Demo::RunMainLoop()
{
	while(true)
	{
		if(!_window->MessagePump())
			break;

		Update(0.0f);
		Draw(0.0f);
	}
}


void Demo::Update(float timeSinceLastUpdate)
{
}

void Demo::Draw(float timeSinceLastUpdate)
{
	Direct3D11Device::Get().ClearBackAndDepthBuffer();
	Direct3D11Device::Get().GetSwapChain()->Present(0, 0);
}