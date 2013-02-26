#include "stdafx.h"
#include "Window.h"
#include "Resource.h"
#include "InputManager.h"

Window::Window(void)
{
}


Window::~Window(void)
{
}

Window* __window;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

	bool alreadyProcessed = InputManager::Get().OnWindowsMessage(message, wParam, lParam);

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

		case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_SIZE:
			if(__window)
			{
				unsigned int newWidth = LOWORD(lParam);
				unsigned int newHeight = HIWORD(lParam);
				for(auto it : __window->_windowResizeCallbacks)
					it.second(newWidth, newHeight);
			}
			break;

		default:
			if(!alreadyProcessed)
				return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


bool Window::InitWindow(HINSTANCE hInstance, const std::wstring& caption, unsigned int sizeX, unsigned int sizeY)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_LARGEDEMO1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"_windowClass";	// :/
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_LARGEDEMO1 );
    if( !RegisterClassEx( &wcex ) )
        return false;

    // Create window
    _hInstance = hInstance;
    RECT rc = { 0, 0, sizeX, sizeY };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	_windowHandle = CreateWindow(wcex.lpszClassName, caption.c_str(), WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if(!_windowHandle)
        return false;

	ShowWindow(_windowHandle, SW_SHOWDEFAULT);

    return true;
}


bool Window::MessagePump()
{
	__window = this;
	InputManager::Get().UpdatePressedReleaseTables();
	MSG msg = {0};
	while(PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
        DispatchMessage( &msg );

		if(msg.message == WM_QUIT)
			return false;
	}

	return true;
}

void Window::RegisterWindowResizeCallback(const std::string& identifier, const std::function<void(unsigned int, unsigned int)>& callback)
{
	_windowResizeCallbacks.insert(std::pair<const std::string, std::function<void(unsigned int, unsigned int)>>(identifier, callback));
}

void Window::UnregisterWindowResizeCallback(const std::string& identifier)
{
	_windowResizeCallbacks.erase(identifier);
}