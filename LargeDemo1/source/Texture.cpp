#include "stdafx.h"
#include "Texture.h"
#include "DeviceManager.h"
#include "D3D11Helper.h"


std::shared_ptr<Texture2D> Texture2D::CreateEmpty(DXGI_FORMAT format, unsigned int width, unsigned int height, TextureCreationFlags creationFlags)
{
	DXGI_SAMPLE_DESC samplingSettings;
	samplingSettings.Count = 1;
	samplingSettings.Quality = 0;
	return CreateEx(nullptr, format, width, height, samplingSettings, creationFlags);
}	

std::shared_ptr<Texture2D> Texture2D::CreateEmpty(DXGI_FORMAT format, unsigned int width, unsigned int height, DXGI_SAMPLE_DESC& samplingSettings, TextureCreationFlags creationFlags)
{
	return CreateEx(nullptr, format, width, height, samplingSettings, creationFlags);
}

std::shared_ptr<Texture2D> Texture2D::CreateFromData(const void* data, DXGI_FORMAT format, unsigned int width, unsigned int height, TextureCreationFlags creationFlags)
{
	DXGI_SAMPLE_DESC samplingSettings;
	samplingSettings.Count = 1;
	samplingSettings.Quality = 0;
	return CreateEx(data, format, width, height, samplingSettings, creationFlags);
}

std::shared_ptr<Texture2D> Texture2D::CreateEx(const void* data, DXGI_FORMAT format, unsigned int width, unsigned int height, DXGI_SAMPLE_DESC& samplingSettings, TextureCreationFlags creationFlags)
{
	assert(DeviceManager::Get().GetContext());

	// needs full mipmap chain, cpu access ("deferred fill") and srv and rendertarget
	if(creationFlags & TextureCreationFlags::AUTOGEN_MIPMAPS)
		creationFlags |= TextureCreationFlags::FULL_MIPMAPCHAIN | TextureCreationFlags::SHADERRES_VIEW;

	unsigned int dataElementSize = D3D11Helper::SizeFromFormat(format);

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = (creationFlags & TextureCreationFlags::FULL_MIPMAPCHAIN) ? 0 : 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count	= samplingSettings.Count;
	desc.SampleDesc.Quality = samplingSettings.Quality;
	desc.Usage = (creationFlags & TextureCreationFlags::AUTOGEN_MIPMAPS || 
				  creationFlags & TextureCreationFlags::RENDERTARGET_VIEW ||
				  creationFlags & TextureCreationFlags::SHADERRES_VIEW) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_IMMUTABLE not possible in case of mipmap generation
	
	desc.BindFlags = 0;
	if(creationFlags & TextureCreationFlags::RENDERTARGET_VIEW || creationFlags & TextureCreationFlags::AUTOGEN_MIPMAPS)	// needed for auto mipmap gen
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	if(creationFlags & TextureCreationFlags::SHADERRES_VIEW)
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if(creationFlags & TextureCreationFlags::UNORDEREDACCESS_VIEW)
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (creationFlags & TextureCreationFlags::AUTOGEN_MIPMAPS) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;


	// inital stuff
	std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> initalData;
	if(data && !(creationFlags & TextureCreationFlags::AUTOGEN_MIPMAPS))
	{
		if(creationFlags & TextureCreationFlags::FULL_MIPMAPCHAIN)
			assert("MipMapChain + initalData + noAutogeneration is not supported yet!");
		else
		{
			initalData.reset(new D3D11_SUBRESOURCE_DATA[1]);
			initalData[0].pSysMem = data;
			initalData[0].SysMemPitch = dataElementSize * width;
			initalData[0].SysMemSlicePitch = initalData[0].SysMemPitch * height;
		}
	}

	// generate texture
	std::shared_ptr<Texture2D> textureObj(new Texture2D());
	assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateTexture2D(&desc, initalData.get(), &textureObj->_texture)));

	// shader view
	if(creationFlags & TextureCreationFlags::SHADERRES_VIEW)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = format;
		srvDesc.ViewDimension = samplingSettings.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1; 
		DeviceManager::Get().GetDevice()->CreateShaderResourceView(textureObj->_texture.p, &srvDesc, &textureObj->_shaderResourceView);
	}
	// rt view
	if(creationFlags & TextureCreationFlags::RENDERTARGET_VIEW)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		memset(&rtDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		rtDesc.Format = format;
		rtDesc.ViewDimension = samplingSettings.Count > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		rtDesc.Texture2D.MipSlice = 0; 
		DeviceManager::Get().GetDevice()->CreateRenderTargetView(textureObj->_texture.p, &rtDesc, &textureObj->_renderTargetView);
	}
	// uav view
	if(creationFlags & TextureCreationFlags::UNORDEREDACCESS_VIEW)
	{
		assert(samplingSettings.Count == 1);
		D3D11_UNORDERED_ACCESS_VIEW_DESC uaDesc;
		memset(&uaDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		uaDesc.Format = format;
		uaDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		DeviceManager::Get().GetDevice()->CreateUnorderedAccessView(textureObj->_texture.p, &uaDesc, &textureObj->_unorderedAccessView);
	}


	// fill with data if not yet happened
	if(data && (creationFlags & TextureCreationFlags::AUTOGEN_MIPMAPS))
	{
		DeviceManager::Get().GetContext()->UpdateSubresource(textureObj->_texture, 0, nullptr, data, dataElementSize * width, dataElementSize * width * height);
		DeviceManager::Get().GetContext()->GenerateMips(textureObj->_shaderResourceView.p);
	}

	return textureObj;
}