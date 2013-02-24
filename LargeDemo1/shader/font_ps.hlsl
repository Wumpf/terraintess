#include "font_gs.hlsl"
#include "globaltexturesampler.hlsl"

Texture2D FontAtlas : register(t0);

float4 main(PS_INPUT input) : SV_TARGET
{
	return FontAtlas.Sample(SamplerTriLinearWrap, input.Tex) * Color;
}