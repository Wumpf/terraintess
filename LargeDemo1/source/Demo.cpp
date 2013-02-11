#include "stdafx.h"
#include "Demo.h"
#include "Window.h"
#include "DeviceManager.h"
#include "Cube.h"

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

	// device
	DXGI_SAMPLE_DESC multiSamplingSettings;
	multiSamplingSettings.Count = 1;
	multiSamplingSettings.Quality = 0;
	DeviceManager::Get().InitDevice(_window->GetHandle(), multiSamplingSettings);

	// 
	_cube.reset(new Cube());

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
	DeviceManager::Get().ClearBackAndDepthBuffer();


	_cube->Draw();


	DeviceManager::Get().GetSwapChain()->Present(0, 0);
}