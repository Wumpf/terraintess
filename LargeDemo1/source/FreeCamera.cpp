#include "stdafx.h"
#include "FreeCamera.h"
#include "InputManager.h"

const float FreeCamera::_rotationSpeed = 0.01f;
const float FreeCamera::_moveSpeed = 8.0f;


FreeCamera::FreeCamera(float fov, float aspectRatio) :
	Camera(fov, aspectRatio),
	_mouseX(0.0f), _mouseY(0.0f)
{
}

FreeCamera::~FreeCamera()
{
}

void FreeCamera::Update(float timeSinceLastUpdate)
{
	_mouseX -= _rotationSpeed * InputManager::Get().GetMouseXDelta();
	_mouseY -= _rotationSpeed * InputManager::Get().GetMouseYDelta();

	_viewDir.x = sinf(_mouseX) * cosf(_mouseY);
	_viewDir.y = sinf(_mouseY);
	_viewDir.z = cosf(_mouseX) * cosf(_mouseY);

	float forward = InputManager::Get().IsKeyboardKeyDownF(Key::W) + InputManager::Get().IsKeyboardKeyDownF(Key::Up) -
					InputManager::Get().IsKeyboardKeyDownF(Key::S) - InputManager::Get().IsKeyboardKeyDownF(Key::Down);
	float right = InputManager::Get().IsKeyboardKeyDownF(Key::D) + InputManager::Get().IsKeyboardKeyDownF(Key::Right) -
					InputManager::Get().IsKeyboardKeyDownF(Key::A) - InputManager::Get().IsKeyboardKeyDownF(Key::Left);

	if(InputManager::Get().IsKeyboardKeyDown(Key::Control))
		forward *= 2.0f;

	_position += (timeSinceLastUpdate * _moveSpeed * forward) * _viewDir;

	_position += (timeSinceLastUpdate * _moveSpeed * right) * _viewDir.Cross(_up);

	UpdateMatrices();
}