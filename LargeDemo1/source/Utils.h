#pragma once

namespace Utils
{
	template<class T> void SafeReleaseCOM(T& object)
	{
		if(object != nullptr)
		{
			object->Release();
			object = nullptr;
		}
	}
}