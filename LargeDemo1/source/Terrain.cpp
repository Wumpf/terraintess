#include "stdafx.h"
#include "Terrain.h"
#include "VertexFormats.h"
#include "Effect.h"
#include "DeviceManager.h"
#include "Camera.h"

#include "RasterizerState.h"
#include "SamplerState.h"

#include "Texture.h"
#include "Utils.h"

#include "PerlinNoiseGenerator.h"

#include "BufferObject.h"

Terrain::Terrain(unsigned int heightmapResolution, unsigned int blockVertexCountSqrt) :
	_blockVertexCountSqrt(blockVertexCountSqrt),
	_effect(new Effect("shader/terrain_vs.cso", Vertices::Position2D::desc, Vertices::Position2D::numDescElements, "shader/terrain_ps.cso",
						"", "shader/terrain_hs.cso", "shader/terrain_ds.cso"))
{
	
	(_blockVertexCountSqrt >= 2);
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
	_blockVertexBuffer.reset(new BufferObject(vertices.get(), numVertices, sizeof(Vertices::Position2D), D3D11_BIND_VERTEX_BUFFER));

    // Create index buffer
	unsigned int numQuadsSqrt =_blockVertexCountSqrt-1;
	unsigned int numIndices = numQuadsSqrt*numQuadsSqrt * 4;
    std::unique_ptr<WORD> indices(new WORD[numIndices]);
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
	_blockIndexBuffer.reset(new BufferObject(indices.get(), numIndices, sizeof(WORD), D3D11_BIND_INDEX_BUFFER));

    // Create the constant buffers
	_patchConstantBuffer.reset(new ConstantBuffer<PatchConstants>());
	PatchConstants& patchConstants = _patchConstantBuffer->GetContent();
	patchConstants.WorldPosition = SimpleMath::Vector2(-5.0f);
	patchConstants.PlaneScale = 20.0f;
	patchConstants.HeightmapTexcoordPosition = SimpleMath::Vector2(0.0f);
	patchConstants.HeightmapTexcoordScale = 0.1f;
	_patchConstantBuffer->UpdateGPUBuffer();

	_terrainConstantBuffer.reset(new ConstantBuffer<TerrainConstants>());
	TerrainConstants& terrainConstants = _terrainConstantBuffer->GetContent();
	terrainConstants.HeightScale = 30.0f;
	terrainConstants.HeightmapTexelSize = 1.0f / heightmapResolution;
	float textureInWorldSize = heightmapResolution * patchConstants.PlaneScale * patchConstants.HeightmapTexcoordScale;
	terrainConstants.HeightmapTexelSizeWorld_doubled = 1.0f * textureInWorldSize *  1.0f / heightmapResolution;	// todo: not doubled atm - better?
	_terrainConstantBuffer->UpdateGPUBuffer();



	// create heightmap
	//_heightmapTexture = Texture2D::CreateFromData(Utils::RandomFloats(1024*1024, 0.0f, 1.0f).get(), DXGI_FORMAT_R32_FLOAT, 1024, 1024);
	PerlinNoiseGenerator noiseGen;
	_heightmapTexture = noiseGen.Generate(heightmapResolution, heightmapResolution, 0.5f, 8);
}


Terrain::~Terrain()
{
}

void Terrain::Draw(const Camera& camera, float totalSize)
{
	auto immediateContext = DeviceManager::Get().GetContext();

	UINT stride = sizeof(Vertices::Position2D);
    UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, _blockVertexBuffer->GetBufferPointer(), &stride, &offset);
	immediateContext->IASetIndexBuffer(_blockIndexBuffer->GetBuffer(), DXGI_FORMAT_R16_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	immediateContext->VSSetConstantBuffers(0, 1, _terrainConstantBuffer->GetBufferPointer());
	immediateContext->DSSetConstantBuffers(0, 1, _terrainConstantBuffer->GetBufferPointer());
	immediateContext->PSSetConstantBuffers(0, 1, _terrainConstantBuffer->GetBufferPointer());

	immediateContext->VSSetConstantBuffers(1, 1, _patchConstantBuffer->GetBufferPointer());
	immediateContext->DSSetConstantBuffers(1, 1, _patchConstantBuffer->GetBufferPointer());
	immediateContext->PSSetConstantBuffers(1, 1, _patchConstantBuffer->GetBufferPointer());


	auto resView = _heightmapTexture->GetShaderResourceView().p;
	immediateContext->VSSetShaderResources(0, 1, &resView);
	immediateContext->PSSetShaderResources(0, 1, &resView);
	immediateContext->DSSetShaderResources(0, 1, &resView);
	
	_effect->Activate();

	DeviceManager::Get().SetRasterizerState(RasterizerState::Wireframe);
	immediateContext->DrawIndexed(_blockIndexBuffer->GetNumElements(), 0, 0);
	DeviceManager::Get().SetRasterizerState(RasterizerState::CullFront);
}