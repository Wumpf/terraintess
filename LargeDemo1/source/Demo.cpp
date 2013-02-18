#include "stdafx.h"
#include "Demo.h"
#include "Window.h"
#include "DeviceManager.h"
#include "Cube.h"
#include "Terrain.h"
#include "Timer.h"
#include "Utils.h"
#include "FreeCamera.h"
#include "InputManager.h"

#include "SamplerState.h"

Demo::Demo() :
	_window(new Window()),
	_passedTimeSinceStart(0.0f),
	_frameNumber(0)
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
	if(!DeviceManager::Get().InitDevice())
		return false;
	DeviceManager::Get().CreateSwapChainAndBackBuffer(_window->GetHandle(), DeviceManager::Get().GetAvailableMultisamplingSettings().back());
	
	_camera.reset(new FreeCamera(Utils::DegToRad(80.0f), DeviceManager::Get().GetBackBufferAspectRatio()));

	// register standard sampler
	DeviceManager::Get().SetSamplerState(SamplerState::PointWrap, 0);
	DeviceManager::Get().SetSamplerState(SamplerState::PointClamp, 1);
	DeviceManager::Get().SetSamplerState(SamplerState::TriLinearWrap, 2);
	DeviceManager::Get().SetSamplerState(SamplerState::TriLinearClamp, 3);
	DeviceManager::Get().SetSamplerState(SamplerState::AnisotropicWrap, 4);
	DeviceManager::Get().SetSamplerState(SamplerState::AnisotropicClamp, 5);


	// 
	_cube.reset(new Cube());
	_terrain.reset(new Terrain(8));

	return true;
}

void Demo::RunMainLoop()
{
	Timer timer;
	float lastFrameLength = 0.0f;

	while(true)
	{
		lastFrameLength = timer.GetTimeSeconds();
		_passedTimeSinceStart += lastFrameLength;
		timer.Reset();


		if(!_window->MessagePump())
			break;

		Update(lastFrameLength);
		++_frameNumber;
		Draw(lastFrameLength);
	}
}


void Demo::Update(float timeSinceLastUpdate)
{
	_camera->Update(timeSinceLastUpdate);
}

void Demo::Draw(float timeSinceLastUpdate)
{
	DeviceManager::Get().ClearBackAndDepthBuffer();


	_cube->Draw(*_camera, _passedTimeSinceStart);
	_terrain->Draw(*_camera, 100.0f);


	DeviceManager::Get().GetSwapChain()->Present(0, 0);
}