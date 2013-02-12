#include "stdafx.h"
#include "Terrain.h"
#include "VertexFormats.h"
#include "Effect.h"
#include "DeviceManager.h"
#include "Camera.h"
#include "RasterizerState.h"

Terrain::Terrain() :
	_effect(new Effect("shader/terrain_vs.cso", Vertices::PositionTexture::desc, Vertices::PositionTexture::numDescElements, "shader/terrain_ps.cso",
						"", "shader/terrain_hs.cso", "shader/terrain_ds.cso"))
{
	    // Create vertex buffer
    Vertices::PositionTexture vertices[] =
    {
        { SimpleMath::Vector3( -1.0f, 0.0f, -1.0f ), SimpleMath::Vector2( 0.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, 0.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, 0.0f, 1.0f ), SimpleMath::Vector2( 1.0f, 1.0f ) },
        { SimpleMath::Vector3( -1.0f, 0.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 1.0f ) },
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(Vertices::PositionTexture) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
	assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_blockVertexBuffer)));

    // Create index buffer
    WORD indices[] =
    {
		0, 1, 2, 3
    };
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(WORD) * 4;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    InitData.pSysMem = indices;
    assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_blockIndexBuffer)));

    // Create the constant buffers
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SimpleMath::Matrix);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, NULL, &_perFrameConstantBuffer)));
}


Terrain::~Terrain()
{
}

void Terrain::Draw(const Camera& camera)
{
	auto immediateContext = DeviceManager::Get().GetImmediateContext();

	UINT stride = sizeof(Vertices::PositionTexture);
    UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &_blockVertexBuffer.p, &stride, &offset);
	immediateContext->IASetIndexBuffer(_blockIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	D3D11_MAPPED_SUBRESOURCE resource;
	immediateContext->Map(_perFrameConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	memcpy(resource.pData, &camera.GetViewProjectionMatrix(), sizeof(SimpleMath::Matrix));
	immediateContext->Unmap(_perFrameConstantBuffer, 0);
	immediateContext->DSSetConstantBuffers(0, 1, &_perFrameConstantBuffer.p);

	_effect->Activate();

	DeviceManager::Get().SetRasterizerState(RasterizerState::Wireframe);
	immediateContext->Draw(4, 0);
	DeviceManager::Get().SetRasterizerState(RasterizerState::CullFront);
}