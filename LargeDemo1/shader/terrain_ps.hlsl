#include "terrain_ds.hlsl"

Texture2D DiffuseTexture : register(t0);

float4 main(DS_OUTPUT input) : SV_TARGET
{
	/*float3 normal = normalize(float3(cos(input.WorldPos.x*20), 1.0f, 0.0f));
	float3 lightVec = normalize(float3(0.8f, 1.0f, 0.8f));
	float NDotL = dot(normal, lightVec);
	return float4(NDotL,NDotL,NDotL, 1.0f);*/

	return DiffuseTexture.Sample(SamplerAnisotropicWrap, input.WorldPos.xz).rrrr;
}