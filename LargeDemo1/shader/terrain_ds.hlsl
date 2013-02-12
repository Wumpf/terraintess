#include "terrain_hs.hlsl"
#include "globalconstantbuffers.hlsl"

struct DS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

[domain("quad")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation, const OutputPatch<HS_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT output;

	output.Pos = float4(lerp(lerp(patch[0].Pos, patch[1].Pos, domain.x),
							 lerp(patch[3].Pos, patch[2].Pos, domain.x), domain.y), 1);
	output.Tex = lerp(lerp(patch[0].Tex, patch[1].Tex, domain.x),
					  lerp(patch[3].Tex, patch[2].Tex, domain.x), domain.y);

	output.Pos = mul(WorldViewProjection, output.Pos);

	return output;
}
