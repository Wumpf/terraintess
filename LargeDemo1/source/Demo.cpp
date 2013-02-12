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

	// 
	_cube.reset(new Cube());
	_terrain.reset(new Terrain());

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
	_terrain->Draw(*_camera);


	DeviceManager::Get().GetSwapChain()->Present(0, 0);
}