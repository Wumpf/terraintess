#include "stdafx.h"
#include "Resource.h"
#include "Demo.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Demo demo;
	if(demo.Initialize(hInstance))
		demo.RunMainLoop();

    return 0;
}
