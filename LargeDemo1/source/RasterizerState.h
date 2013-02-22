#pragma once

class RasterizerState
{
friend class DeviceManager;

public:
	const D3D11_RASTERIZER_DESC& operator -> () const { return _desc; }
	const D3D11_RASTERIZER_DESC& GetDescription() const { return _desc; }

	RasterizerState(D3D11_FILL_MODE FillMode = D3D11_FILL_SOLID, D3D11_CULL_MODE CullMode = D3D11_CULL_BACK, bool FrontCounterClockwise = false, int DepthBias = 0, float DepthBiasClamp = 0.0f,
						float SlopeScaledDepthBias = 0.0f, bool DepthClipEnable = true, bool ScissorEnable = false, bool MultisampleEnable = false, bool AntialiasedLineEnable = false);

	static RasterizerState CullNone;
	static RasterizerState CullFront;
	static RasterizerState CullBack;
	static RasterizerState Wireframe;
	static RasterizerState WireframeFrontOnly;

private:
	D3D11_RASTERIZER_DESC _desc;
	CComPtr<ID3D11RasterizerState> _stateObject;	///< device manager will create this if needed
};