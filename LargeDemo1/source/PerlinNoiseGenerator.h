#pragma once

class PerlinNoiseGenerator
{
public:
	PerlinNoiseGenerator();

	std::shared_ptr<class Texture2D> Generate(unsigned int width, unsigned int height, float persistance, unsigned int octaveCount);

private:
	std::shared_ptr<class ComputeShader> _computeShader;
	static const unsigned int NUM_THREADS_PER_GROUP;
};