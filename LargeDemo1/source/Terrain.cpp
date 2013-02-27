#include "stdafx.h"
#include "Terrain.h"
#include "Effect.h"
#include "DeviceManager.h"
#include "Camera.h"

#include "RasterizerState.h"
#include "SamplerState.h"

#include "Texture.h"
#include "Utils.h"

#include "PerlinNoiseGenerator.h"

#include "BufferObject.h"

struct TerrainVertex
{
	SimpleMath::Vector2 pos2D;
	float skirtFactor;	// 1 for skirt vertices

	static const D3D11_INPUT_ELEMENT_DESC desc[];
	static const unsigned int numDescElements;
};	
const D3D11_INPUT_ELEMENT_DESC TerrainVertex::desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SKIRT", 0, DXGI_FORMAT_R32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
const unsigned int TerrainVertex::numDescElements = sizeof(TerrainVertex::desc) / sizeof(TerrainVertex::desc[0]);


Terrain::Terrain(float totalTerrainSize, unsigned int heightmapResolution, float pixelPerTriangle, float detailHeightmapTexcoordFactor, 
				float heightmapYScale, float detailHeightmapYScale,  unsigned int patchCountPerBlockSqrt) :
	_heightmapResolution(heightmapResolution),
	_totalTerrainSize(totalTerrainSize),
	_minimumWorldBlockSize(patchCountPerBlockSqrt * totalTerrainSize / heightmapResolution),	// try to give every vertex a heightmap-texel
	_pixelPerTriangle(pixelPerTriangle),
	_wireframe(false),
	_effect(new Effect("shader/terrain_vs.cso", TerrainVertex::desc, TerrainVertex::numDescElements, "shader/terrain_ps.cso",
						"", "shader/terrain_hs.cso", "shader/terrain_ds.cso"))
{
	assert(patchCountPerBlockSqrt >= 1);

	// Create vertex buffer
	unsigned int numMainVerticesSqrt = patchCountPerBlockSqrt+1;
	unsigned int numMainVertices = (patchCountPerBlockSqrt+1)*(patchCountPerBlockSqrt+1);
	unsigned int numVertices = numMainVertices + (patchCountPerBlockSqrt+1) * 1;
	assert(numVertices < 0xFFFF);
	std::unique_ptr<TerrainVertex[]> vertices(new TerrainVertex[numVertices]);
	// main vertices
	TerrainVertex* vertex = vertices.get();
	for(unsigned int y=0; y<patchCountPerBlockSqrt+1; ++y)
	{
		for(unsigned int x=0; x<patchCountPerBlockSqrt+1; ++x)
		{
			vertex->pos2D.x = static_cast<float>(x) / patchCountPerBlockSqrt;
			vertex->pos2D.y = static_cast<float>(y) / patchCountPerBlockSqrt;
			vertex->skirtFactor = 0.0f;
			++vertex;
		}
	}
	// skirt vertices
	float skirtFactor = detailHeightmapYScale * 0.25f;
		// top
	for(unsigned int i=0; i<patchCountPerBlockSqrt+1; ++i)
	{
		vertex->pos2D.x = static_cast<float>(i) / patchCountPerBlockSqrt;
		vertex->pos2D.y = 0.0f;
		vertex->skirtFactor = skirtFactor;
		++vertex;
	}
		// left
/*		for(unsigned int i=0; i<patchCountPerBlockSqrt+1; ++i)
	{
		vertex->pos2D.x = 1.0f;
		vertex->pos2D.y = static_cast<float>(i) / patchCountPerBlockSqrt;
		vertex->skirtFactor = 1.0f;
		++vertex;
	}
		// bottom
for(unsigned int i=0; i<patchCountPerBlockSqrt+1; ++i)
	{
		vertex->pos2D.x = static_cast<float>(i) / patchCountPerBlockSqrt;
		vertex->pos2D.y = 1.0f;
		vertex->skirtFactor = 1.0f;
		++vertex;
	}
		// right
	for(unsigned int i=0; i<patchCountPerBlockSqrt+1; ++i)
	{
		vertex->pos2D.x = 0.0f;
		vertex->pos2D.y = static_cast<float>(i) / patchCountPerBlockSqrt;
		vertex->skirtFactor = 1.0f;
		++vertex;
	}*/

	_blockVertexBuffer.reset(new BufferObject(vertices.get(), numVertices, sizeof(TerrainVertex), D3D11_BIND_VERTEX_BUFFER));

    // Create index buffer
	unsigned int numIndices = (patchCountPerBlockSqrt*patchCountPerBlockSqrt + patchCountPerBlockSqrt * 1) * 4;
    std::unique_ptr<WORD> indices(new WORD[numIndices]);
	// main quads
	WORD* index = indices.get();
	for(unsigned int y=0; y<patchCountPerBlockSqrt; ++y)
	{
		for(unsigned int x=0; x<patchCountPerBlockSqrt; ++x)
		{
			index[0] = x + y * (patchCountPerBlockSqrt+1);
			index[1] = index[0] + 1;
			index[2] = index[1] + (patchCountPerBlockSqrt+1);
			index[3] = index[0] + (patchCountPerBlockSqrt+1);
			index += 4;
		}
	}
	// skirt quads
	unsigned int skirtVertexIndex = numMainVertices;
		// top
	for(unsigned int i=0; i<patchCountPerBlockSqrt; ++i)
	{
		index[0] = skirtVertexIndex++;
		index[1] = index[0] + 1;
		index[2] = i+1;
		index[3] = i;
		index += 4;
	}
/*		// left
	for(unsigned int i=0; i<patchCountPerBlockSqrt; ++i)
	{
		index[0] = numMainVerticesSqrt * (i+1);
		index[1] = skirtVertexIndex++;
		index[2] = skirtVertexIndex;
		index[3] = index[0] + numMainVerticesSqrt;
		index += 4;
	}
		// bottom
	for(unsigned int i=0; i<patchCountPerBlockSqrt; ++i)
	{
		vertex->pos2D.x = static_cast<float>(i) / patchCountPerBlockSqrt;
		vertex->pos2D.y = 1.0f;
		vertex->skirtFactor = 0.0f;
		++vertex;
	}
		// right
	for(unsigned int i=0; i<patchCountPerBlockSqrt; ++i)
	{
		vertex->pos2D.x = 0.0f;
		vertex->pos2D.y = static_cast<float>(i) / patchCountPerBlockSqrt;
		vertex->skirtFactor = 0.0f;
		++vertex;
	}*/
	_blockIndexBuffer.reset(new BufferObject(indices.get(), numIndices, sizeof(WORD), D3D11_BIND_INDEX_BUFFER));

    // Create the constant buffers
	_patchConstantBuffer.reset(new ConstantBuffer<PatchConstants>());
	_terrainConstantBuffer.reset(new ConstantBuffer<TerrainConstants>());
	TerrainConstants& terrainConstants = _terrainConstantBuffer->GetContent();

	terrainConstants.CoarseHeightScale = heightmapYScale;
	terrainConstants.CoarseHeightmapTexelSize = 1.0f / heightmapResolution;
	terrainConstants.TrianglesPerClipSpaceUnit = (DeviceManager::Get().GetBackBufferWidth() / _pixelPerTriangle) / 2.0f;
	terrainConstants.CoarseHeightmapTexelSizeWorld_doubled = 2.0f * _totalTerrainSize / heightmapResolution;	// heightmapPixelSizeInWorldCordinates
	
	terrainConstants.DetailHeightScale = detailHeightmapYScale;
	terrainConstants.DetailHeightmapTexcoordFactor = detailHeightmapTexcoordFactor;
	terrainConstants.DetailHeightmapTexelSize = 1.0f / DETAIL_HEIGHTMAP_RESOLUTION;
	terrainConstants.DetailHeightmapTexelSizeWorld_doubled = terrainConstants.CoarseHeightmapTexelSizeWorld_doubled * detailHeightmapTexcoordFactor;

	_terrainConstantBuffer->UpdateGPUBuffer();



	// create heightmap
	PerlinNoiseGenerator noiseGen;
	_heightmapCoarseTexture = noiseGen.Generate(heightmapResolution, heightmapResolution, 0.4f, 8);
	_heightmapDetailTexture = noiseGen.Generate(DETAIL_HEIGHTMAP_RESOLUTION, DETAIL_HEIGHTMAP_RESOLUTION, 0.5f, 6);
}


Terrain::~Terrain()
{
}

void Terrain::OnBackBufferResize(unsigned int height, unsigned int width)
{
	TerrainConstants& terrainConstants = _terrainConstantBuffer->GetContent();
	terrainConstants.TrianglesPerClipSpaceUnit = width / _pixelPerTriangle / 2.0f;
	_terrainConstantBuffer->UpdateGPUBuffer();
}

void Terrain::DrawRecursive(const SimpleMath::Vector2& min, const SimpleMath::Vector2& max, const SimpleMath::Vector2& cameraPos2D)
{
	SimpleMath::Vector2 center = (min + max) / 2;
	float blockSize = max.x - min.x;
	float distanceToCamSq = SimpleMath::Vector2::DistanceSquared(center, cameraPos2D);
	if(blockSize > distanceToCamSq)
		distanceToCamSq = 0.0f;

	if(false)	// culling
		return;
	
	
	if(distanceToCamSq / (blockSize * blockSize) > 6.0f ||	// camera is more than 2x current block size distant - far enough away to rendert NOW
		blockSize <= _minimumWorldBlockSize)	// minimum size
	{
		PatchConstants& content = _patchConstantBuffer->GetContent();
		content.WorldPosition = min;
		content.PlaneScale = blockSize;
		content.HeightmapTexcoordScale = blockSize / _totalTerrainSize;
		content.HeightmapTexcoordPosition = SimpleMath::Vector2(min / _totalTerrainSize) + SimpleMath::Vector2(0.5f);
		_patchConstantBuffer->UpdateGPUBuffer();
		
		DeviceManager::Get().GetContext()->DrawIndexed(_blockIndexBuffer->GetNumElements(), 0, 0);
	}
	else // subdivide
	{
		DrawRecursive(min, center, cameraPos2D);
		DrawRecursive(SimpleMath::Vector2(center.x, min.y), SimpleMath::Vector2(max.x, center.y) , cameraPos2D);
		DrawRecursive(center, max, cameraPos2D);
		DrawRecursive(SimpleMath::Vector2(min.x, center.y), SimpleMath::Vector2(center.x, max.y) , cameraPos2D);
	}
}

void Terrain::Draw(const Camera& camera, float totalSize)
{
	auto immediateContext = DeviceManager::Get().GetContext();

//	auto immediateContext = DeviceManager::Get().GetContext();


	UINT stride = sizeof(TerrainVertex);
    UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, _blockVertexBuffer->GetBufferPointer(), &stride, &offset);
	immediateContext->IASetIndexBuffer(_blockIndexBuffer->GetBuffer(), DXGI_FORMAT_R16_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	immediateContext->VSSetConstantBuffers(0, 1, _terrainConstantBuffer->GetBufferPointer());
	immediateContext->DSSetConstantBuffers(0, 1, _terrainConstantBuffer->GetBufferPointer());
	immediateContext->PSSetConstantBuffers(0, 1, _terrainConstantBuffer->GetBufferPointer());
	immediateContext->HSSetConstantBuffers(0, 1, _terrainConstantBuffer->GetBufferPointer());

	immediateContext->VSSetConstantBuffers(1, 1, _patchConstantBuffer->GetBufferPointer());	
	immediateContext->DSSetConstantBuffers(1, 1, _patchConstantBuffer->GetBufferPointer());
	immediateContext->PSSetConstantBuffers(1, 1, _patchConstantBuffer->GetBufferPointer());

	ID3D11ShaderResourceView* heightmapView[] = { _heightmapCoarseTexture->GetShaderResourceView().p, _heightmapDetailTexture->GetShaderResourceView().p };
	immediateContext->VSSetShaderResources(0, 2, heightmapView);
	immediateContext->PSSetShaderResources(0, 2, heightmapView);
	immediateContext->DSSetShaderResources(0, 2, heightmapView);

	_effect->Activate();


	if(_wireframe)
		DeviceManager::Get().SetRasterizerState(RasterizerState::WireframeFrontOnly);
	DrawRecursive(SimpleMath::Vector2(-_totalTerrainSize*0.5f), SimpleMath::Vector2(_totalTerrainSize*0.5f), SimpleMath::Vector2(camera.GetPosition().x, camera.GetPosition().z));
	if(_wireframe)
		DeviceManager::Get().SetRasterizerState(RasterizerState::CullBack);
}