#include "stdafx.h"
#include "Terrain.h"
#include "VertexFormats.h"
#include "Effect.h"
#include "DeviceManager.h"
#include "Camera.h"

#include "RasterizerState.h"
#include "SamplerState.h"

#include "NoiseGenerator.h"
#include "Texture.h"

Terrain::Terrain(unsigned int blockVertexCountSqrt) :
	_blockVertexCountSqrt(blockVertexCountSqrt),
	_effect(new Effect("shader/terrain_vs.cso", Vertices::Position2D::desc, Vertices::Position2D::numDescElements, "shader/terrain_ps.cso",
						"", "shader/terrain_hs.cso", "shader/terrain_ds.cso"))
{
	assert(_blockVertexCountSqrt >= 2);
	assert(_blockVertexCountSqrt*_blockVertexCountSqrt < 0xFFFF);

	// Create vertex buffer
	unsigned int numVertices = _blockVertexCountSqrt*_blockVertexCountSqrt;
	std::unique_ptr<Vertices::Position2D[]> vertices(new Vertices::Position2D[numVertices]);
	Vertices::Position2D* vertex = vertices.get();
	for(unsigned int y=0; y<_blockVertexCountSqrt; ++y)
	{
		for(unsigned int x=0; x<_blockVertexCountSqrt; ++x)
		{
			vertex->pos2D.x = static_cast<float>(x) / (_blockVertexCountSqrt-1);
			vertex->pos2D.y = static_cast<float>(y) / (_blockVertexCountSqrt-1);
			++vertex;
		}
	}
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
	bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(Vertices::Position2D) * numVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices.get();
	InitData.SysMemSlicePitch = InitData.SysMemPitch = 0;
	assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_blockVertexBuffer)));

    // Create index buffer
	unsigned int numQuadsSqrt =_blockVertexCountSqrt-1;
	_numIndices = numQuadsSqrt*numQuadsSqrt * 4;
    std::unique_ptr<WORD> indices(new WORD[_numIndices]);
	WORD* index = indices.get();
	for(unsigned int y=0; y<numQuadsSqrt; ++y)
	{
		for(unsigned int x=0; x<numQuadsSqrt; ++x)
		{
			index[0] = x + y * _blockVertexCountSqrt;
			index[1] = index[0] + 1;
			index[2] = index[1] + _blockVertexCountSqrt;
			index[3] = index[0] + _blockVertexCountSqrt;
			index += 4;
		}
	}
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = _numIndices * sizeof(WORD);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    InitData.pSysMem = indices.get();
    assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, &InitData, &_blockIndexBuffer)));

    // Create the constant buffers
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(SimpleMath::Matrix);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&bd, NULL, &_perFrameConstantBuffer)));


	// create heightmap
	_heightmapTexture = Texture2D::StandardTextureFromData(NoiseGenerator::WhiteNoise_Float(1024, 1024, 0.0f, 1.0f).get(), DXGI_FORMAT_R32_FLOAT, 1024, 1024);
}


Terrain::~Terrain()
{
}

void Terrain::Draw(const Camera& camera, float totalSize)
{
	auto immediateContext = DeviceManager::Get().GetImmediateContext();

	UINT stride = sizeof(Vertices::Position2D);
    UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &_blockVertexBuffer.p, &stride, &offset);
	immediateContext->IASetIndexBuffer(_blockIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	D3D11_MAPPED_SUBRESOURCE resource;
	immediateContext->Map(_perFrameConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	SimpleMath::Matrix worldViewProjection = SimpleMath::Matrix::CreateTranslation(-0.5f, -7.0f, -0.5f) * 
							SimpleMath::Matrix::CreateScale(totalSize, 1.0f, totalSize) * camera.GetViewProjectionMatrix();
	memcpy(resource.pData, &worldViewProjection, sizeof(SimpleMath::Matrix));
	immediateContext->Unmap(_perFrameConstantBuffer, 0);
	immediateContext->DSSetConstantBuffers(0, 1, &_perFrameConstantBuffer.p);

	auto resView = _heightmapTexture->GetRessourceView().p;
	immediateContext->PSSetShaderResources(0, 1, &resView);
	
	DeviceManager::Get().SetSamplerState(SamplerState::LinearWrap, Shader::Type::PIXEL, 0);
	_effect->Activate();

//	DeviceManager::Get().SetRasterizerState(RasterizerState::Wireframe);
	immediateContext->DrawIndexed(_numIndices, 0, 0);
//	DeviceManager::Get().SetRasterizerState(RasterizerState::CullFront);
}