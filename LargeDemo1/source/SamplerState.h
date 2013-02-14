#pragma once

class SamplerState
{
friend class DeviceManager;

public:
	const D3D11_SAMPLER_DESC& operator -> () const { return _desc; }
	const D3D11_SAMPLER_DESC& GetDescription() const { return _desc; }

	SamplerState(D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_MODE adressMode = D3D11_TEXTURE_ADDRESS_WRAP, UINT maxAnisotropy = 16,
					float borderColor[] = nullptr, float minLod = 0.0f, float maxLod = D3D11_FLOAT32_MAX, D3D11_COMPARISON_FUNC comparisionFunc = D3D11_COMPARISON_NEVER);

	static SamplerState LinearWrap;

private:
	D3D11_SAMPLER_DESC _desc;
	CComPtr<ID3D11SamplerState> _stateObject;	///< device manager will create this if needed
};