#include "test_vs.hlsl"

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(input.Tex, 0.0f, 1.0f);
}