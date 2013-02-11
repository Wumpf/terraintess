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

	void ErrorMsg(const std::string& message);

	extern const double pi;

	template<class T> T RadToDeg(T rad) { return rad * static_cast<T>(180.0 / pi); }
	template<class T> T DegToRad(T deg) { return deg * static_cast<T>(pi / 180.0); }
}