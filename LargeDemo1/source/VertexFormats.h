#pragma once

namespace Vertices
{
	struct PositionTexture
	{
		SimpleMath::Vector3 pos;
		SimpleMath::Vector2 tex;

		static const D3D11_INPUT_ELEMENT_DESC desc[];
		static const unsigned int numDescElements;
	};
}