#include "stdafx.h"
#include "Camera.h"


Camera::Camera(float fov, float aspectRatio) :
	_projectionMatrix(SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, 0.1f, 1000.0f)),
	_position(-15.0f, 0.0f, 0.0f),
	 _up(0.0f, 1.0f, 0.0f),
	_viewDir(1.0f, 0.0f, 0.0f)
{
	UpdateMatrices();
}

Camera::~Camera(void)
{
}

void Camera::UpdateMatrices()
{
	_viewMatrix = SimpleMath::Matrix::CreateLookAt(_position, _position + _viewDir , _up);
	_viewProjectionMatrix = _viewMatrix * _projectionMatrix;
}

