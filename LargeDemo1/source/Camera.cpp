#include "stdafx.h"
#include "Camera.h"

#include "BufferObject.h"
#include "DeviceManager.h"

Camera::Camera(float fov, float aspectRatio) :
	_position(0.0f, 200.0f, 0.0f),
	 _up(0.0f, 1.0f, 0.0f),
	_viewDir(1.0f, 0.0f, 0.0f),

	_constantBuffer(new ConstantBuffer<Camera::Constants>())
{
	_constantBuffer->GetContent()._projectionMatrix = SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, 0.1f, 5000.0f);
	

	UpdateMatrices();
}

Camera::~Camera(void)
{
}

void Camera::UpdateMatrices()
{
	Camera::Constants& constants = _constantBuffer->GetContent();

	constants._viewMatrix = SimpleMath::Matrix::CreateLookAt(_position, _position + _viewDir , _up);
	constants._viewProjectionMatrix = constants._viewMatrix * constants._projectionMatrix;
	
}

void Camera::UpdateGPUBuffer()
{
	_constantBuffer->GetContent()._position = _position;
	_constantBuffer->UpdateGPUBuffer();
}

void Camera::ActivateCameraConstantBufferForAllShader(unsigned int bufferSlotIndex)
{
	DeviceManager::Get().GetContext()->CSSetConstantBuffers(bufferSlotIndex, 1, _constantBuffer->GetBufferPointer());
	DeviceManager::Get().GetContext()->GSSetConstantBuffers(bufferSlotIndex, 1, _constantBuffer->GetBufferPointer());
	DeviceManager::Get().GetContext()->PSSetConstantBuffers(bufferSlotIndex, 1, _constantBuffer->GetBufferPointer());
	DeviceManager::Get().GetContext()->VSSetConstantBuffers(bufferSlotIndex, 1, _constantBuffer->GetBufferPointer());
	DeviceManager::Get().GetContext()->HSSetConstantBuffers(bufferSlotIndex, 1, _constantBuffer->GetBufferPointer());
	DeviceManager::Get().GetContext()->DSSetConstantBuffers(bufferSlotIndex, 1, _constantBuffer->GetBufferPointer());
}

const SimpleMath::Matrix& Camera::GetProjectionMatrix()	const		{ return _constantBuffer->GetContent()._projectionMatrix; }
const SimpleMath::Matrix& Camera::GetViewMatrix() const				{ return _constantBuffer->GetContent()._viewMatrix; }
const SimpleMath::Matrix& Camera::GetViewProjectionMatrix() const	{ return _constantBuffer->GetContent()._viewProjectionMatrix; }