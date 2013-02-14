#include "stdafx.h"
#include "NoiseGenerator.h"
#include "Utils.h"

namespace NoiseGenerator
{
	std::unique_ptr<float[]> WhiteNoise_Float(unsigned int width, unsigned int height, float min, float max)
	{
		unsigned int numValues = width*height;
		std::unique_ptr<float[]> whiteNoise(new float[numValues]);
		float* end = whiteNoise.get() + numValues;
		for(float* val=whiteNoise.get(); val<end; ++val)
			*val = static_cast<float>(Utils::Random(min, max));

		return whiteNoise;
	}
};