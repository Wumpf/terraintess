
struct VS_INPUT
{
    float2 Pos2D : POSITION;
};

struct HS_INPUT
{
    float2 Pos2D : POSITION;
};

HS_INPUT main(VS_INPUT input)
{
    HS_INPUT output = (HS_INPUT)0;
    output.Pos2D = input.Pos2D;
    return output;
}
