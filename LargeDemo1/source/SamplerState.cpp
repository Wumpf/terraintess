#include "stdafx.h"
#include "SamplerState.h"

SamplerState SamplerState::LinearWrap(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP);

SamplerState::SamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE adressMode, UINT maxAnisotropy,
						float borderColor[], float minLod, float maxLod, D3D11_COMPARISON_FUNC comparisionFunc)
{
	_desc.Filter = filter;
	_desc.AddressU = adressMode;
	_desc.AddressV = adressMode;
	_desc.AddressW = adressMode;
	_desc.ComparisonFunc = comparisionFunc;
	_desc.MaxAnisotropy = maxAnisotropy;
	if(!borderColor)
		_desc.BorderColor[0] = _desc.BorderColor[1] = _desc.BorderColor[2] = _desc.BorderColor[3] = 0.0f;
	else
		memcpy(_desc.BorderColor, borderColor, sizeof(float) * 4);
	_desc.MinLOD = minLod;
	_desc.MaxLOD = maxLod;
}

