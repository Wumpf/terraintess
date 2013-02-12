
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct HS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output = (HS_INPUT)0;
    output.Pos = input.Pos;
    output.Tex = input.Tex;
   
    return output;
}
