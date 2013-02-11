#include "stdafx.h"
#include "Cube.h"
#include "DeviceManager.h"
#include "Utils.h"
#include "Effect.h"

struct SimpleVertex
{
	SimpleMath::Vector3 pos;
    SimpleMath::Vector2 tex;

	static const D3D11_INPUT_ELEMENT_DESC desc[];
};
const D3D11_INPUT_ELEMENT_DESC SimpleVertex::desc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };


Cube::Cube() :
	_effect(new Effect("shader/test_vs.cso", SimpleVertex::desc, sizeof(SimpleVertex::desc) / sizeof(SimpleVertex::desc[0]), "shader/test_ps.cso"))
{
    // Create vertex buffer
    SimpleVertex vertices[] =
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
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
	HRESULT hr = DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_vertexBuffer);
	assert(SUCCEEDED(hr));

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

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_indexBuffer);
    assert(SUCCEEDED(hr));

    // Create the constant buffers
	ID3D11Buffer* constantBuffer;
    bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleMath::Matrix);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = DeviceManager::Get().GetDevice()->CreateBuffer(&bd, NULL, &constantBuffer);
	assert(SUCCEEDED(hr));

    // Initialize the world matrices
	float aspectRatio = static_cast<float>(DeviceManager::Get().GetBackBufferWidth()) / DeviceManager::Get().GetBackBufferHeight();
	SimpleMath::Matrix matrix = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(-15.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f), SimpleMath::Vector3(0.0f, 1.0f, 0.0f)) *
									 SimpleMath::Matrix::CreatePerspectiveFieldOfView(Utils::DegToRad(80.0f), aspectRatio, 0.1f, 1000.0f);

	//matrix = matrix.Transpose();
	DeviceManager::Get().GetImmediateContext()->UpdateSubresource(constantBuffer, 0, NULL, &matrix, 0, 0);


	DeviceManager::Get().GetImmediateContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
}


Cube::~Cube()
{
}

void Cube::Draw()
{
	auto immediateContext = DeviceManager::Get().GetImmediateContext();

	// Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &_vertexBuffer.p, &stride, &offset);

	// set index buffer
	immediateContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_effect->Activate();
    immediateContext->DrawIndexed( 36, 0, 0 );
}
