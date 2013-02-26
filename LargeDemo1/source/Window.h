#pragma once

/// \remarks due to a messy message pump hack only one window will work at a time
class Window
{
friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

	// resize observer
	void RegisterWindowResizeCallback(const std::string& identifier, const std::function<void(unsigned int, unsigned int)>& callback);
	void UnregisterWindowResizeCallback(const std::string& identifier);

private:
	HWND _windowHandle;
	HINSTANCE _hInstance;

	std::unordered_map<std::string, std::function<void(unsigned int, unsigned int)>> _windowResizeCallbacks;
};

