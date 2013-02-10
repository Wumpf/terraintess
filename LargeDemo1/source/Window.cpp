#include "stdafx.h"
#include "Window.h"
#include "Resource.h"

Window::Window(void)
{
}


Window::~Window(void)
{
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
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
