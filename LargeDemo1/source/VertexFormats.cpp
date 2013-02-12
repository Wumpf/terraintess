#include "stdafx.h"
#include "VertexFormats.h"

namespace Vertices
{
	const D3D11_INPUT_ELEMENT_DESC PositionTexture::desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	const unsigned int PositionTexture::numDescElements = sizeof(PositionTexture::desc) / sizeof(PositionTexture::desc[0]);

}