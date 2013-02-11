#pragma once

#include "Camera.h"

class FreeCamera : public Camera
{
public:
	FreeCamera(float fov, float aspectRatio);
	virtual ~FreeCamera();

	virtual void Update(float timeSinceLastUpdate);

private:
	static const float _rotationSpeed;
	static const float _moveSpeed;

	float _mouseX, _mouseY;
};

