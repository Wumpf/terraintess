#include "stdafx.h"
#include "Camera.h"


Camera::Camera(float fov, float aspectRatio)
{
	_projectionMatrix = SimpleMath::Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, 0.1f, 1000.0f);
	_viewMatrix = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(-15.0f, 0.0f, 0.0f), SimpleMath::Vector3(0.0f), SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
	_viewProjectionMatrix = _viewMatrix * _projectionMatrix;
}


Camera::~Camera(void)
{
}
