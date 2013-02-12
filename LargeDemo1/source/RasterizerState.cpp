#include "stdafx.h"
#include "RasterizerState.h"


RasterizerState RasterizerState::CullNone(D3D11_FILL_SOLID, D3D11_CULL_NONE);
RasterizerState RasterizerState::CullFront(D3D11_FILL_SOLID, D3D11_CULL_FRONT);
RasterizerState RasterizerState::CullBack(D3D11_FILL_SOLID, D3D11_CULL_BACK);
RasterizerState RasterizerState::Wireframe(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE);


RasterizerState::RasterizerState(D3D11_FILL_MODE FillMode, D3D11_CULL_MODE CullMode, bool FrontCounterClockwise, int DepthBias, float DepthBiasClamp,
						float SlopeScaledDepthBias, bool DepthClipEnable, bool ScissorEnable, bool MultisampleEnable, bool AntialiasedLineEnable)
{
	_desc.FillMode = FillMode;
	_desc.CullMode = CullMode;
	_desc.FrontCounterClockwise = FrontCounterClockwise;
	_desc.DepthBias = DepthBias;
	_desc.DepthBiasClamp = DepthBiasClamp;
	_desc.SlopeScaledDepthBias = SlopeScaledDepthBias;
	_desc.DepthClipEnable = DepthClipEnable;
	_desc.ScissorEnable = ScissorEnable;
	_desc.MultisampleEnable = MultisampleEnable;
	_desc.AntialiasedLineEnable = AntialiasedLineEnable;
}

