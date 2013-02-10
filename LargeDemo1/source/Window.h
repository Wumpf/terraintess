#pragma once

class Window
{
public:
	Window();
	~Window();

	/// Initializes a window
	/// \return true if everything is alright, false otherwise
	bool InitWindow(HINSTANCE hInstance, const std::wstring& caption, unsigned int sizeX, unsigned int sizeY);

	/// performs message translation and dispatching
	/// \return true if everything is alright, false if quit or error
	bool MessagePump();

	HWND GetHandle() { return _windowHandle; }

private:
	HWND _windowHandle;
	HINSTANCE _hInstance;
};

