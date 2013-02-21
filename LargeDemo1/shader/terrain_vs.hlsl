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
	output.WorldPos.y = Heightmap.SampleLevel(SamplerPointWrap, output.HeightmapCoord, 0).r * HeightScale;	// mipmap?

	//float4 projectedPos = mul(ViewProjection, float4(output.WorldPos, 1.0f));
	//output.WorldPosOnScreen = projectedPos.xy / projectedPos.w;

    return output;
}
