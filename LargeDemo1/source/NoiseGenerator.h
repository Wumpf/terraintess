#pragma once

namespace NoiseGenerator
{
	std::unique_ptr<float[]> WhiteNoise_Float(unsigned int width, unsigned int height, float min, float max);
};

