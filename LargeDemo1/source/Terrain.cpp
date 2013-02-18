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
	_perFrameConstantBuffer = BufferObject::CreateConstantBuffer(sizeof(SimpleMath::Matrix));


	// create heightmap
	//_heightmapTexture = Texture2D::CreateFromData(Utils::RandomFloats(1024*1024, 0.0f, 1.0f).get(), DXGI_FORMAT_R32_FLOAT, 1024, 1024);
	PerlinNoiseGenerator noiseGen;
	_heightmapTexture = noiseGen.Generate(1024, 1024);
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
	
	SimpleMath::Matrix worldViewProjection = SimpleMath::Matrix::CreateTranslation(-0.5f, -7.0f, -0.5f) * 
							SimpleMath::Matrix::CreateScale(totalSize, 1.0f, totalSize) * camera.GetViewProjectionMatrix();
	_perFrameConstantBuffer->Write(&worldViewProjection);

	immediateContext->DSSetConstantBuffers(0, 1, _perFrameConstantBuffer->GetBufferPointer());

	auto resView = _heightmapTexture->GetShaderResourceView().p;
	immediateContext->PSSetShaderResources(0, 1, &resView);
	immediateContext->DSSetShaderResources(0, 1, &resView);
	
	_effect->Activate();

//	DeviceManager::Get().SetRasterizerState(RasterizerState::Wireframe);
	immediateContext->DrawIndexed(_blockIndexBuffer->GetNumElements(), 0, 0);
//	DeviceManager::Get().SetRasterizerState(RasterizerState::CullFront);
}