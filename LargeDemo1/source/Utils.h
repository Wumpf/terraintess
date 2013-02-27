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

	template<class T> T Clamp(T value, T min, T max)
	{
		if(value < min)
			return min;
		else if(value > max)
			return max;
		else
			return value;
	}

	void InitRandom(uint32_t seed);
	uint32_t Random();
	double Random(double min, double max);
	std::unique_ptr<float[]> RandomFloats(unsigned int numValues, float min, float max);

	// enables "flag behaviour" for typed enums
#define CLASS_ENUM_FLAG(type) \
	namespace { \
		type operator | (type lhs, type rhs) \
		{ return (type)((uint32_t)lhs | (uint32_t)rhs); } \
		type operator |= (type& lhs, type rhs) \
		{ return lhs = static_cast<type>((uint32_t)lhs | (uint32_t)rhs); } \
		bool operator & (type lhs, type rhs) \
		{  return ((uint32_t)lhs & (uint32_t)rhs) > 0; } \
	}
};