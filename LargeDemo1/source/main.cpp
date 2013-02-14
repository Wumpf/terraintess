#include "stdafx.h"
#include "Resource.h"
#include "Demo.h"
#include "Utils.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_SYSTEMTIME time;
	GetLocalTime(&time);
	Utils::InitRandom(time.wMilliseconds);

	Demo demo;
	if(demo.Initialize(hInstance))
		demo.RunMainLoop();

    return 0;
}
