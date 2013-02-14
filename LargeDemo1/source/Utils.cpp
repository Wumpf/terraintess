#include "stdafx.h"
#include "Utils.h"

namespace Utils
{
	const double pi = 3.14159265;

	void ErrorMsg(const std::string& message) {
		MessageBoxA(NULL, message.c_str(), "Error", MB_OK | MB_ICONERROR);
	}


	 
	static uint32_t PHI = 0x9e3779b9;
	static uint32_t Q[4096], c = 362436;
 
	void InitRandom(uint32_t seed)
	{
			int i;
			Q[0] = seed;
			Q[1] = seed + PHI;
			Q[2] = seed + PHI + PHI;
 
			for (i = 3; i < 4096; i++)
					Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
	}
 
	uint32_t Random()
	{
			uint64_t t, a = 18782LL;
			static uint32_t i = 4095;
			uint32_t x, r = 0xfffffffe;
			i = (i + 1) & 4095;
			t = a * Q[i] + c;
			c = (t >> 32);
			x = static_cast<uint32_t>(t + c);
			if (x < c) {
					x++;
					c++;
			}
			return (Q[i] = r - x);
	}

	double Random(double min, double max)
	{
		return (static_cast<double>(Random()) / UINT32_MAX) * max + min;
	}
};