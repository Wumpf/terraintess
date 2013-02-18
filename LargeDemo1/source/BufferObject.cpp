#include "stdafx.h"
#include "BufferObject.h"
#include "DeviceManager.h"

BufferObject::BufferObject(const void* initalData, unsigned int elementSize, D3D11_BIND_FLAG bindings, D3D11_CPU_ACCESS_FLAG cpuAccess, D3D11_USAGE usage) :
	BufferObject(initalData, 1, elementSize, bindings, cpuAccess, usage)
{
}

BufferObject::BufferObject(const void* initalData, unsigned int numElements, unsigned int elementSize, D3D11_BIND_FLAG bindings, D3D11_CPU_ACCESS_FLAG cpuAccess, D3D11_USAGE usage) :
	_numElements(numElements)
{
	assert(cpuAccess == 0 || usage != D3D11_USAGE_IMMUTABLE);
	assert(initalData != nullptr || usage != D3D11_USAGE_IMMUTABLE);

	// D3D11 ERROR: ID3D11Device::CreateBuffer: The Dimensions are invalid. For ConstantBuffers, marked with the D3D11_BIND_CONSTANT_BUFFER BindFlag, the ByteWidth (value = 8) must be a multiple of 16. [ STATE_CREATION ERROR #66: CREATEBUFFER_INVALIDDIMENSIONS]
	unsigned int bufferSizeBytes = numElements * elementSize;
	if(bindings & D3D11_BIND_CONSTANT_BUFFER)
		bufferSizeBytes += bufferSizeBytes % 16;

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
		assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&_bufferDesc, &InitData, &_bufferResource)));
	}
	else
		assert(SUCCEEDED(DeviceManager::Get().GetDevice()->CreateBuffer(&_bufferDesc, nullptr, &_bufferResource)));
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
		DeviceManager::Get().GetContext()->UpdateSubresource(_bufferResource.p, 0, nullptr, data, dataSize, 0);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		DeviceManager::Get().GetContext()->Map(_bufferResource.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, data, dataSize);
		DeviceManager::Get().GetContext()->Unmap(_bufferResource.p, 0);
	}
}