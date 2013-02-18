#pragma once

#include "Utils.h"

/*
enum class BufferBindType
{
	CONSTANT = 0xF000,
	VERTEX = 0x0F00,
	INDEX = 0x00F0,
	UNORDERED = 0x000F
};
CLASS_ENUM_FLAG(BufferBindType)
*/

class BufferObject
{
public:
	static std::shared_ptr<BufferObject> CreateConstantBuffer(unsigned int size)
	{ return std::shared_ptr<BufferObject>(new BufferObject(nullptr, 1, size, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC));}
	
	BufferObject(const void* initalData, unsigned int elementSize, D3D11_BIND_FLAG bindings, D3D11_CPU_ACCESS_FLAG cpuAccess = (D3D11_CPU_ACCESS_FLAG)0, D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE);
	BufferObject(const void* initalData, unsigned int numElements, unsigned int elementSize, D3D11_BIND_FLAG bindings, D3D11_CPU_ACCESS_FLAG cpuAccess = (D3D11_CPU_ACCESS_FLAG)0, D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE);
	~BufferObject();

	template<class T> void Write(const T* data) { Write(data, sizeof(T)); }
	void Write(const void* data, unsigned int dataSize);
	// TODO: partial writes missing


	ID3D11Buffer* const * GetBufferPointer() const	{ return &_bufferResource.p; }
	const CComPtr<ID3D11Buffer>& GetBuffer() const { return _bufferResource; }
	CComPtr<ID3D11Buffer> GetBuffer() { return _bufferResource; }

	unsigned int GetNumElements() const { return _numElements; }

private:
	D3D11_BUFFER_DESC _bufferDesc;
	CComPtr<ID3D11Buffer> _bufferResource;
	unsigned int _numElements;
};

