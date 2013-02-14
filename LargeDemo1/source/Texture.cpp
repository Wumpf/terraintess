#include "stdafx.h"
#include "Texture.h"
#include "DeviceManager.h"

std::unique_ptr<Texture2D> Texture2D::StandardTextureFromData(void* data, unsigned int dataElementSize, DXGI_FORMAT format, unsigned int width, unsigned int height)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	int numLevels = 1;
	while((width > 1) || (height > 1))
	{
    	width = max(width / 2, 1);
    	height = max(height / 2, 1);
    	++numLevels;
	}

	std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> subresourceData(new D3D11_SUBRESOURCE_DATA[numLevels]);
	for(int i=0; i<numLevels; ++i)
	{
		subresourceData[i].pSysMem = data;
		subresourceData[i].SysMemPitch = dataElementSize * width;
		subresourceData[i].SysMemSlicePitch = 0;
	}



	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0; 
	srvDesc.Texture2D.MipLevels = -1;


	std::unique_ptr<Texture2D> textureObj(new Texture2D());
	DeviceManager::Get().GetDevice()->CreateTexture2D(&desc, subresourceData.get(), &textureObj->_texture);
	DeviceManager::Get().GetDevice()->CreateShaderResourceView(textureObj->_texture.p, &srvDesc, &textureObj->_ressourceView);

	return textureObj;
}
