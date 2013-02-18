#include "terrain_hs.hlsl"
#include "globalconstantbuffers.hlsl"
#include "globaltexturesampler.hlsl"

Texture2D HeightTexture : register(t0);

struct DS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 WorldPos : NORMAL;
};

[domain("quad")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation, const OutputPatch<HS_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT output;

	output.WorldPos.xz = lerp(lerp(patch[0].Pos2D, patch[1].Pos2D, domain.x),
						 lerp(patch[3].Pos2D, patch[2].Pos2D, domain.x), domain.y);
	output.WorldPos.y = HeightTexture.SampleLevel(SamplerPointWrap, output.WorldPos.xz, 0).r * 5;

	//HeightmapTexture.
	
	output.Pos = mul(WorldViewProjection, float4(output.WorldPos, 1.0f));

	return output;
}
