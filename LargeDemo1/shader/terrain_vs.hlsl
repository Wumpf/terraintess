#include "globaltexturesampler.hlsl"
#include "globalconstantbuffers.hlsl"
#include "terrain_constants.hlsl"

struct VS_INPUT
{
    float2 Pos2D : POSITION;
};

struct HS_INPUT
{
    float3 WorldPos : POSITION;
	float2 HeightmapCoord : TEXCOORD;
};

HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output = (HS_INPUT)0;
    output.WorldPos.xz = input.Pos2D * PlaneScale + WorldPosition;

	output.HeightmapCoord = input.Pos2D * HeightmapTexcoordScale + HeightmapTexcoordPosition;

	// HeightmapTexcoordScale seems to be mipmap we want! if its 0 then we have infinite detail, if its one we have zero detail; 
	// problem: it never gets to 0, only close :/
	// but looks good so far
	output.WorldPos.y = CoarseHeightmap.SampleLevel(SamplerPointWrap, output.HeightmapCoord, HeightmapTexcoordScale).r * CoarseHeightScale +
						DetailHeightmap.SampleLevel(SamplerTriLinearWrap, output.HeightmapCoord * DetailHeightmapTexcoordFactor, 0.0f).r * DetailHeightScale;	

	//float4 projectedPos = mul(ViewProjection, float4(output.WorldPos, 1.0f));
	//output.WorldPosOnScreen = projectedPos.xy / projectedPos.w;

    return output;
}
