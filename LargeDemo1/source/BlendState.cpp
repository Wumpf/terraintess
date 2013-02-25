#include "stdafx.h"
#include "BlendState.h"

BlendState BlendState::Additive(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
								D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
								D3D11_COLOR_WRITE_ENABLE_ALL, true);
BlendState BlendState::AlphaBlend(D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
								  D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
								  D3D11_COLOR_WRITE_ENABLE_ALL, true);
BlendState BlendState::Opaque(D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
								D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
								D3D11_COLOR_WRITE_ENABLE_ALL, false);

BlendState::BlendState(D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp,
							D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_BLEND_OP BlendOpAlpha,
							UINT8 RenderTargetWriteMask, bool blending)
{
	_desc.AlphaToCoverageEnable = true;
	_desc.IndependentBlendEnable = false;
	_desc.RenderTarget[0].SrcBlend = SrcBlend;
	_desc.RenderTarget[0].DestBlend = DestBlend;
	_desc.RenderTarget[0].BlendOp = BlendOp;
	_desc.RenderTarget[0].SrcBlendAlpha = SrcBlendAlpha;
	_desc.RenderTarget[0].DestBlendAlpha = DestBlendAlpha;
	_desc.RenderTarget[0].BlendOpAlpha = BlendOpAlpha;
	_desc.RenderTarget[0].RenderTargetWriteMask = RenderTargetWriteMask;
}

