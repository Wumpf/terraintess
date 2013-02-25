#pragma once

class BlendState
{
friend class DeviceManager;

public:
	const D3D11_BLEND_DESC& operator -> () const { return _desc; }
	const D3D11_BLEND_DESC& GetDescription() const { return _desc; }

	BlendState(D3D11_BLEND SrcBlend = D3D11_BLEND_SRC_ALPHA, D3D11_BLEND DestBlend = D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP BlendOp = D3D11_BLEND_OP_ADD,
				D3D11_BLEND SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA, D3D11_BLEND DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP BlendOpAlpha = D3D11_BLEND_OP_ADD,
				UINT8 RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL, bool blending = true);

	// missing: independant settings for mrt
	// missing: alpha to coverage off (its always on)

	static BlendState Additive;
	static BlendState AlphaBlend;
	static BlendState Opaque;

private:
	D3D11_BLEND_DESC _desc;
	CComPtr<ID3D11BlendState> _stateObject;	///< device manager will create this if needed
};