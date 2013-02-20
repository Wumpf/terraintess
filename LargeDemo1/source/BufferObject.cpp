#include "stdafx.h"
#include "BufferObject.h"
#include "DeviceManager.h"

BufferObject::BufferObject(const void* initalData, unsigned int elementSize, D3D11_BIND_FLAG bindings, D3D11_CPU_ACCESS_FLAG cpuAccess, D3D11_USAGE usage)
{
	Init(initalData, 1, elementSize, bindings, cpuAccess, usage);
}

BufferObject::BufferObject(const void* initalData, unsigned int numElements, unsigned int elementSize, D3D11_BIND_FLAG bindings, D3D11_CPU_ACCESS_FLAG cpuAccess, D3D11_USAGE usage)
{
	Init(initalData, numElements, elementSize, bindings, cpuAccess, usage);
}

void BufferObject::Init(const void* initalData, unsigned int numElements, unsigned int elementSize, D3D11_BIND_FLAG bindings, D3D11_CPU_ACCESS_FLAG cpuAccess, D3D11_USAGE usage)
{
	assert(cpuAccess == 0 || usage != D3D11_USAGE_IMMUTABLE);
	assert(initalData != nullptr || usage != D3D11_USAGE_IMMUTABLE);

	_numElements = numElements;

	// D3D11 ERROR: ID3D11Device::CreateBuffer: The Dimensions are invalid. For ConstantBuffers, marked with the D3D11_BIND_CONSTANT_BUFFER BindFlag, the ByteWidth (value = 8) must be a multiple of 16. [ STATE_CREATION ERROR #66: CREATEBUFFER_INVALIDDIMENSIONS]
	unsigned int bufferSizeBytes = numElements * elementSize;
	if(bindings & D3D11_BIND_CONSTANT_BUFFER)
		bufferSizeBytes += (16 - bufferSizeBytes % 16) % 16;

	_bufferDesc.ByteWidth = bufferSizeBytes;
	_bufferDesc.Usage = usage;
    _bufferDesc.BindFlags = bindings;
	_bufferDesc.CPUAccessFlags = cpuAccess;
	_bufferDesc.StructureByteStride = elementSize;
	_bufferDesc.MiscFlags = 0;

	if(initalData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = initalData;
		InitData.SysMemSlicePitch = InitData.SysMemPitch = 0;
		HRESULT hr = DeviceManager::Get().GetDevice()->CreateBuffer(&_bufferDesc, &InitData, &_bufferResource);
		assert(SUCCEEDED(hr));
	}
	else
	{
		HRESULT hr = DeviceManager::Get().GetDevice()->CreateBuffer(&_bufferDesc, nullptr, &_bufferResource);
		assert(SUCCEEDED(hr));
	}
}

BufferObject::~BufferObject(void)
{
}

void BufferObject::Write(const void* data, unsigned int dataSize)
{
	assert(_bufferDesc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE);
	assert(_bufferDesc.Usage != D3D11_USAGE_IMMUTABLE);
	assert(_bufferDesc.ByteWidth >= dataSize);

	if(_bufferDesc.Usage == D3D11_USAGE_DEFAULT)
	{
		DeviceManager::Get().GetContext()->UpdateSubresource(_bufferResource, 0, nullptr, data, dataSize, 0);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		DeviceManager::Get().GetContext()->Map(_bufferResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy_s(resource.pData, dataSize, data, dataSize);
		DeviceManager::Get().GetContext()->Unmap(_bufferResource, 0);
	}
}