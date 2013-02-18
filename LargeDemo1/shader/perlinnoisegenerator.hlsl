#include "globaltexturesampler.hlsl"

#define BLOCK_SIZE 512

Texture2D NoiseTexture : register(t0);	// tiling, simple 2d adressing | simplificated, no mip
RWTexture2D<float> PerlinNoiseOut : register(u0);

cbuffer config : register(b0)
{
	float2 TextureSize;
	float Persistance;
	uint OctaveCount;
}

float2 perlinInterpolation(float2 _x)
{
    // C2-continual implementation
    // For details see "Burger-GradientNoiseGerman-2008".
    return _x * _x * _x * (_x * (_x * 6.0f - 15.0f) + 10.0f);
}

[numthreads(BLOCK_SIZE, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float amplitude = 1.0f;
	float totalAmplitude = 0.0f;
	float output = 0.0f;

	

	for(int octave = OctaveCount - 1; octave >= 0; --octave)	// starting with biggest octave
    {
		int samplePeriod = 1 << octave; // calculates 2 ^ k
		float sampleFrequency = 1.0f / samplePeriod;

		int2 samplePos = (DTid.xy / samplePeriod) * samplePeriod;
		float2 interp = perlinInterpolation((DTid.xy - samplePos) * sampleFrequency);

		float2 texcoord = samplePos / TextureSize;
		float2 samplePeriodTex = float2(samplePeriod, samplePeriod) / TextureSize;

		float4 gatheredValues;// = NoiseTexture.GatherRed(SamplerPointWrap, samplePos / TextureSize, int2(0, 0), int2(0, samplePeriod), int2(samplePeriod, 0), int2(samplePeriod, samplePeriod));
		gatheredValues.x = NoiseTexture.SampleLevel(SamplerTriLinearWrap, texcoord, 0.0f).r;
		gatheredValues.y = NoiseTexture.SampleLevel(SamplerTriLinearWrap, texcoord + float2(0, samplePeriodTex.y), 0.0f).r;
		gatheredValues.z = NoiseTexture.SampleLevel(SamplerTriLinearWrap, texcoord + float2(samplePeriodTex.x, 0), 0.0f).r;
		gatheredValues.w = NoiseTexture.SampleLevel(SamplerTriLinearWrap, texcoord + samplePeriodTex, 0.0f).r;


		float tempL = lerp(gatheredValues.x, gatheredValues.y, interp.y);
		float tempR = lerp(gatheredValues.z, gatheredValues.w, interp.y);
	    output += lerp(tempL, tempR, interp.x) * amplitude;

		totalAmplitude += amplitude;
		amplitude *= Persistance;
    }

	PerlinNoiseOut[DTid.xy] = output / totalAmplitude;
}