#include "stdafx.h"
#include "InputManager.h"


InputManager::InputManager() :
	mouseXDelta(0),
	mouseYDelta(0),
	mouseX(0),
	mouseY(0),
	mouseWheel(0),
	mouseWheelDelta(0)
{
	for(int i=0; i<Key::NUM_KEYS; ++i)
		keyPressed[i] = false;
	for(int i=0; i<Key::NUM_KEYS; ++i)
		keyReleased[i] = false;
	for(int i=0; i<Key::NUM_KEYS; ++i)
		keyDown[i] = false;

	for(int i=0; i<MouseButton::NUM_BUTTONS; ++i)
		mouseButtonPressed[i] = false;
	for(int i=0; i<MouseButton::NUM_BUTTONS; ++i)
		mouseButtonReleased[i] = false;
	for(int i=0; i<MouseButton::NUM_BUTTONS; ++i)
		mouseButtonDown[i] = false;
}

InputManager::~InputManager()
{
}

void InputManager::UpdatePressedReleaseTables()
{
	for(int i=0; i<Key::NUM_KEYS; ++i)
		keyReleased[i] = false;
	for(int i=0; i<Key::NUM_KEYS; ++i)
		keyPressed[i] = false;

	for(int i=0; i<MouseButton::NUM_BUTTONS; ++i)
		mouseButtonPressed[i] = false;
	for(int i=0; i<MouseButton::NUM_BUTTONS; ++i)
		mouseButtonReleased[i] = false;
}

#define GETX(l) (int(l & 0xFFFF))
#define GETY(l) (int(l) >> 16)

bool InputManager::OnWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		mouseXDelta = GETX(lParam) - mouseX;
		mouseYDelta = GETY(lParam) - mouseY;
		mouseX = GETX(lParam);
		mouseY = GETY(lParam);
		break;

	case WM_KEYDOWN:
		keyPressed[wParam] = true;
		keyDown[wParam] = true;
		break;
	case WM_KEYUP:
		keyReleased[wParam] = true;
		keyDown[wParam] = false;
		break;

	case WM_LBUTTONDOWN:
		mouseButtonPressed[MouseButton::LEFT] = true;
		mouseButtonDown[MouseButton::LEFT] = true;
 		break;
	case WM_LBUTTONUP:
		keyReleased[MouseButton::LEFT] = true;
		mouseButtonDown[MouseButton::LEFT] = false;
 		break;
	case WM_RBUTTONDOWN:
		mouseButtonPressed[MouseButton::RIGHT] = true;
		mouseButtonDown[MouseButton::RIGHT] = true;
 		break;
	case WM_RBUTTONUP:
		keyReleased[MouseButton::RIGHT] = true;
		mouseButtonDown[MouseButton::RIGHT] = false;
 		break;
	case WM_MBUTTONDOWN:
		mouseButtonPressed[MouseButton::MIDDLE] = true;
		mouseButtonDown[MouseButton::MIDDLE] = true;
 		break;
	case WM_MBUTTONUP:
		keyReleased[MouseButton::MIDDLE] = true;
		mouseButtonDown[MouseButton::MIDDLE] = false;
 		break;

	case WM_MOUSEWHEEL:
		mouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
		mouseWheel += mouseWheelDelta;
		break;

	default:
		return false;
	};

	return true;
}