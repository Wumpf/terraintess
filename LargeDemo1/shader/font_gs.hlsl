#include "font_vs.hlsl"
#include "globalconstantbuffers.hlsl"

cbuffer FontSettings : register(b0)
{
	float Height_clipSpace : packoffset(c0.x);
	float Height_texSpace : packoffset(c0.y);
	float2 FontAtlasSize : packoffset(c0.z);
	float4 Color : packoffset(c1.x);
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

[maxvertexcount(4)]
void main(point CHARVERTEX vertex[1], inout TriangleStream<PS_INPUT> quad)
{
	PS_INPUT output;

	float2 upperLeft_clipSpace = float2(-1.0f, 1.0f) + vertex[0].Pos_pixel / ScreenSize * float2(2.0f, -2.0f);
	float width_clipSpace = vertex[0].Width_pixel / ScreenSize.x * 2.0f;

	float2 upperLeft_texture = vertex[0].TexPos_pixel / FontAtlasSize;
	float width_texture = vertex[0].Width_pixel / FontAtlasSize.x;

	output.Pos.zw = float2(0.0f, 1.0f);
	output.Tex = float2(0.0f, 0.0f);

	// bottom left
	output.Pos.xy = upperLeft_clipSpace + float2(0.0f, -Height_clipSpace);
	output.Tex = upperLeft_texture + float2(0.0f, Height_texSpace);
	quad.Append(output);

	// top left
	output.Pos.xy = upperLeft_clipSpace;
	output.Tex = upperLeft_texture;
	quad.Append(output);

	// bottom right
	output.Pos.xy = upperLeft_clipSpace + float2(width_clipSpace, -Height_clipSpace);
	output.Tex = upperLeft_texture + float2(width_texture, Height_texSpace);
	quad.Append(output);

	// top right
	output.Pos.xy = upperLeft_clipSpace + float2(width_clipSpace, 0.0f);
	output.Tex = upperLeft_texture + float2(width_texture, 0.0f);
	quad.Append(output);
}
