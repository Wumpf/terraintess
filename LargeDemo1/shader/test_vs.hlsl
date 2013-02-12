#include "globalconstantbuffers.hlsl"

struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

PS_INPUT main( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(WorldViewProjection, float4(input.Pos, 1.0f));
    output.Tex = input.Tex;
   
    return output;
}
