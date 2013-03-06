#include "stdafx.h"
#include "Texture.h"
#include "DeviceManager.h"
#include "D3D11Helper.h"
#include "stb_image.h"

std::shared_ptr<Texture> Texture::CreateFromFile(const std::string& filename, CreationFlags creationFlags)
{
	int comp;
	int width, height;
	std::unique_ptr<stbi_uc> data(stbi_load(filename.c_str(), &width, &height, &comp, 4));

	if(!data)
	{
		assert(false && "Couldn't load texture file");
		return nullptr;
	}

	return CreateFromData(data.get(), DXGI_FORMAT_R8G8B8A8_UNORM, width, height, creationFlags);
}

std::shared_ptr<Texture> Texture::CreateArrayFromFiles(const std::vector<std::string> filenames, CreationFlags creationFlags)
{
	int lastWidth = -1;
	int lastHeight = -1;

	std::unique_ptr<unsigned char> arrayData;
	unsigned char* nextTextureElementPtr = nullptr;
	int textureSizeBytes = -1;

	for(const std::string& filename : filenames)
	{
		int comp;
		int width, height;
		std::unique_ptr<stbi_uc> data(stbi_load(filename.c_str(), &width, &height, &comp, 4));

		if(!data)
		{
			assert(false && "Couldn't load texture file");
			return nullptr;
		}
		if(lastWidth != -1 && (lastWidth != width || lastHeight != height))
		{
			assert(false && "Texture sizes of texture array are not the same!");
			return nullptr;
		}
		lastWidth = width;
		lastHeight = height;

		if(nextTextureElementPtr == nullptr)
		{
			textureSizeBytes = 4 * height * width;
			arrayData.reset(new unsigned char[textureSizeBytes * filenames.size()]);
			nextTextureElementPtr = arrayData.get();
		}

		memcpy(nextTextureElementPtr, data.get(), textureSizeBytes);
		nextTextureElementPtr += textureSizeBytes;
	}

	DXGI_SAMPLE_DESC samplingSettings;
	samplingSettings.Count = 1;
	samplingSettings.Quality = 0;
	return CreateEx(arrayData.get(), DXGI_FORMAT_R8G8B8A8_UNORM, lastWidth, lastHeight, static_cast<unsigned int>(filenames.size()), samplingSettings, creationFlags);
}

std::shared_ptr<Texture> Texture::CreateEmpty(DXGI_FORMAT format, unsigned int width, unsigned int height, CreationFlags creationFlags)
{
	DXGI_SAMPLE_DESC samplingSettings;
	samplingSettings.Count = 1;
	samplingSettings.Quality = 0;
	return CreateEx(nullptr, format, width, height, 1, samplingSettings, creationFlags);
}	

std::shared_ptr<Texture> Texture::CreateEmpty(DXGI_FORMAT format, unsigned int width, unsigned int height, DXGI_SAMPLE_DESC& samplingSettings, CreationFlags creationFlags)
{
	return CreateEx(nullptr, format, width, height, 1, samplingSettings, creationFlags);
}

std::shared_ptr<Texture> Texture::CreateFromData(const void* data, DXGI_FORMAT format, unsigned int width, unsigned int height, CreationFlags creationFlags)
{
	DXGI_SAMPLE_DESC samplingSettings;
	samplingSettings.Count = 1;
	samplingSettings.Quality = 0;
	return CreateEx(data, format, width, height, 1, samplingSettings, creationFlags);
}

std::shared_ptr<Texture> Texture::CreateEx(const void* data, DXGI_FORMAT format, unsigned int width, unsigned int height, unsigned int arraySize, DXGI_SAMPLE_DESC& samplingSettings, CreationFlags creationFlags)
{
	assert(DeviceManager::Get().GetContext());
	assert(arraySize != 0);

	// needs full mipmap chain, cpu access ("deferred fill") and srv and rendertarget
	if(creationFlags & CreationFlags::AUTOGEN_MIPMAPS)
		creationFlags |= CreationFlags::FULL_MIPMAPCHAIN | CreationFlags::SHADERRES_VIEW;

	unsigned int dataElementSize = D3D11Helper::SizeFromFormat(format);

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = (creationFlags & CreationFlags::FULL_MIPMAPCHAIN) ? 0 : 1;
	desc.ArraySize = arraySize;
	desc.Format = format;
	desc.SampleDesc.Count	= samplingSettings.Count;
	desc.SampleDesc.Quality = samplingSettings.Quality;
	desc.Usage = (creationFlags & CreationFlags::AUTOGEN_MIPMAPS || 
				  creationFlags & CreationFlags::RENDERTARGET_VIEW ||
				  creationFlags & CreationFlags::SHADERRES_VIEW) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;	// D3D11_USAGE_IMMUTABLE not possible in case of mipmap generation
	
	desc.BindFlags = 0;
	if(creationFlags & CreationFlags::RENDERTARGET_VIEW || creationFlags & CreationFlags::AUTOGEN_MIPMAPS)	// needed for auto mipmap gen
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	if(creationFlags & CreationFlags::SHADERRES_VIEW)
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if(creationFlags & CreationFlags::UNORDEREDACCESS_VIEW)
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (creationFlags & CreationFlags::AUTOGEN_MIPMAPS) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;


	// inital stuff
	std::unique_ptr<D3D11_SUBRESOURCE_DATA[]> initalData;
	if(data && !(creationFlags & CreationFlags::AUTOGEN_MIPMAPS))
	{
		if(creationFlags & CreationFlags::FULL_MIPMAPCHAIN)
			assert(nullptr); //"MipMapChain + initalData + noAutogeneration is not supported yet!");
		else
		{
			initalData.reset(new D3D11_SUBRESOURCE_DATA[arraySize]);
			for(unsigned int i=0; i<arraySize; ++i)
			{
				initalData[i].pSysMem = static_cast<const unsigned char*>(data) + i * dataElementSize * width * height;
				initalData[i].SysMemPitch = dataElementSize * width;
				initalData[i].SysMemSlicePitch = initalData[i].SysMemPitch * height;
			}
		}
	}

	// generate texture
	std::shared_ptr<Texture> textureObj(new Texture());
	HRESULT hr = DeviceManager::Get().GetDevice()->CreateTexture2D(&desc, initalData.get(), &textureObj->_texture);
	assert(SUCCEEDED(hr));

	// shader view
	if(creationFlags & CreationFlags::SHADERRES_VIEW)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = format;
		if(arraySize == 1)
			srvDesc.ViewDimension = samplingSettings.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		else
		{
			srvDesc.ViewDimension = samplingSettings.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = arraySize;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
		}
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2DArray.ArraySize = arraySize;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		DeviceManager::Get().GetDevice()->CreateShaderResourceView(textureObj->_texture.p, &srvDesc, &textureObj->_shaderResourceView);
	}
	// rt view
	if(creationFlags & CreationFlags::RENDERTARGET_VIEW)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		memset(&rtDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		rtDesc.Format = format;
		if(arraySize == 1)
			rtDesc.ViewDimension = samplingSettings.Count > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		else
		{
			rtDesc.ViewDimension = samplingSettings.Count > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtDesc.Texture2DArray.ArraySize = arraySize;
			rtDesc.Texture2DArray.FirstArraySlice = 0;
		}
		rtDesc.Texture2D.MipSlice = 0;
		DeviceManager::Get().GetDevice()->CreateRenderTargetView(textureObj->_texture.p, &rtDesc, &textureObj->_renderTargetView);
	}
	// uav view
	if(creationFlags & CreationFlags::UNORDEREDACCESS_VIEW)
	{
		assert(samplingSettings.Count == 1);
		D3D11_UNORDERED_ACCESS_VIEW_DESC uaDesc;
		memset(&uaDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		uaDesc.Format = format;
		uaDesc.ViewDimension = arraySize > 1 ? D3D11_UAV_DIMENSION_TEXTURE2DARRAY : D3D11_UAV_DIMENSION_TEXTURE2D;
		uaDesc.Texture2DArray.ArraySize = arraySize;
		uaDesc.Texture2DArray.FirstArraySlice = 0;
		DeviceManager::Get().GetDevice()->CreateUnorderedAccessView(textureObj->_texture.p, &uaDesc, &textureObj->_unorderedAccessView);
	}


	// fill with data if not yet happened
	if(data && (creationFlags & CreationFlags::AUTOGEN_MIPMAPS))
	{
		unsigned int numberOfMipMaps = 1 + static_cast<unsigned int>(log10(static_cast<float>(max(width, height))) / log10(2.0));
		unsigned int textureSizeBytes = dataElementSize * width * height;
		for(unsigned int i=0; i<arraySize; ++i)
		{
			DeviceManager::Get().GetContext()->UpdateSubresource(textureObj->_texture, D3D11CalcSubresource(0, i, numberOfMipMaps), nullptr, 
									static_cast<const unsigned char*>(data) + textureSizeBytes * i, dataElementSize * width, textureSizeBytes);
		}
		DeviceManager::Get().GetContext()->GenerateMips(textureObj->_shaderResourceView.p);
	}

	return textureObj;
}