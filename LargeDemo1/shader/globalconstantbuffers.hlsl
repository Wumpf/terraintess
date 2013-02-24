

cbuffer CameraConstants : register(b5)
{
	matrix ViewProjection;
	matrix Projection;
	matrix View;
	float3 CameraPosition;
};

cbuffer ScreenConstants : register(b6)
{
	float2 ScreenSize;
	float AspectRatio;
};

#define CameraX View[0].xyz
#define CameraY View[1].xyz
#define CameraZ View[2].xyz