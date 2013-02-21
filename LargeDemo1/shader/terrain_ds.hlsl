#include "terrain_hs.hlsl"

struct DS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float3 WorldPos : NORMAL;
	float2 HeightmapCoord : TEXCOORD0;
};

[domain("quad")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation, const OutputPatch<HS_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT output;

	output.WorldPos = lerp(lerp(patch[0].WorldPos, patch[1].WorldPos, domain.x),
						 lerp(patch[3].WorldPos, patch[2].WorldPos, domain.x), domain.y);


	
	output.HeightmapCoord = lerp(lerp(patch[0].HeightmapCoord, patch[1].HeightmapCoord, domain.x),
						 lerp(patch[3].HeightmapCoord, patch[2].HeightmapCoord, domain.x), domain.y);

	output.WorldPos.y = Heightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord, 0.0f).r * HeightScale;	// todo mipmap dependent on vertex distance


	output.Pos = mul(ViewProjection, float4(output.WorldPos, 1.0f));

	return output;
}
