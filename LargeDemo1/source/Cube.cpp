#include "stdafx.h"
#include "Cube.h"
#include "DeviceManager.h"
#include "Utils.h"
#include "Effect.h"
#include "Camera.h"
#include "InputManager.h"
#include "VertexFormats.h"


Cube::Cube() :
	_effect(new Effect("shader/test_vs.cso", Vertices::PositionTexture::desc, Vertices::PositionTexture::numDescElements, "shader/test_ps.cso"))
{
    // Create vertex buffer
    Vertices::PositionTexture vertices[] =
    {
        { SimpleMath::Vector3( -1.0f, 1.0f, -1.0f ), SimpleMath::Vector2( 0.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, 1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, 1.0f, 1.0f ), SimpleMath::Vector2( 1.0f, 1.0f ) },
        { SimpleMath::Vector3( -1.0f, 1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 1.0f ) },

        { SimpleMath::Vector3( -1.0f, -1.0f, -1.0f ), SimpleMath::Vector2( 0.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, -1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, -1.0f, 1.0f ), SimpleMath::Vector2( 1.0f, 1.0f ) },
        { SimpleMath::Vector3( -1.0f, -1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 1.0f ) },

        { SimpleMath::Vector3( -1.0f, -1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 0.0f ) },
        { SimpleMath::Vector3( -1.0f, -1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 0.0f ) },
        { SimpleMath::Vector3( -1.0f, 1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 1.0f ) },
        { SimpleMath::Vector3( -1.0f, 1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 1.0f ) },

        { SimpleMath::Vector3( 1.0f, -1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, -1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, 1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 1.0f ) },
        { SimpleMath::Vector3( 1.0f, 1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 1.0f ) },

        { SimpleMath::Vector3( -1.0f, -1.0f, -1.0f ), SimpleMath::Vector2( 0.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, -1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, 1.0f, -1.0f ), SimpleMath::Vector2( 1.0f, 1.0f ) },
        { SimpleMath::Vector3( -1.0f, 1.0f, -1.0f ), SimpleMath::Vector2( 0.0f, 1.0f ) },

        { SimpleMath::Vector3( -1.0f, -1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, -1.0f, 1.0f ), SimpleMath::Vector2( 1.0f, 0.0f ) },
        { SimpleMath::Vector3( 1.0f, 1.0f, 1.0f ), SimpleMath::Vector2( 1.0f, 1.0f ) },
        { SimpleMath::Vector3( -1.0f, 1.0f, 1.0f ), SimpleMath::Vector2( 0.0f, 1.0f ) },
    };



    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertices::PositionTexture) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
	assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_vertexBuffer)));

    // Create index buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
    };

    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(WORD) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    InitData.pSysMem = indices;
    assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_indexBuffer)));

    // Create the constant buffers
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SimpleMath::Matrix);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, NULL, &_constantBuffer)));
}


Cube::~Cube()
{
}

void Cube::Draw(const Camera& camera, float totalPassedTime)
{
	auto immediateContext = DeviceManager::Get().GetContext();

	UINT stride = sizeof(Vertices::PositionTexture);
    UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &_vertexBuffer.p, &stride, &offset);
	immediateContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_MAPPED_SUBRESOURCE resource;
	immediateContext->Map(_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	SimpleMath::Matrix worldViewProjection = SimpleMath::Matrix::CreateFromYawPitchRoll(InputManager::Get().GetMouseWheel() * 0.0001f, 0.0f, 0.0f) * camera.GetViewProjectionMatrix();
	memcpy(resource.pData, &worldViewProjection, sizeof(SimpleMath::Matrix));
	immediateContext->Unmap(_constantBuffer, 0);
	immediateContext->VSSetConstantBuffers(0, 1, &_constantBuffer.p);

	_effect->Activate();
    immediateContext->DrawIndexed( 36, 0, 0 );
}
