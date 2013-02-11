#include "stdafx.h"
#include "Utils.h"

namespace Utils
{
	const double pi = 3.14159265;

	void ErrorMsg(const std::string& message) {
		MessageBoxA(NULL, message.c_str(), "Error", MB_OK | MB_ICONERROR);
	}
};