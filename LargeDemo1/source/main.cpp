#include "stdafx.h"
#include "Resource.h"
#include "Demo.h"
#include "Utils.h"

#include <time.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	time_t t;
	time(&t);
	Utils::InitRandom(static_cast<uint32_t>(t));

	Demo demo;
	if(demo.Initialize(hInstance))
		demo.RunMainLoop();

    return 0;
}
