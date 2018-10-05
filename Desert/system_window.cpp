#include "system_window.h"
#include "log.h"


namespace Desert::System {
	LRESULT Window::StaticProcessor(HWND h, UINT m, WPARAM w, LPARAM l)
	{
		//Debug::Info("HWND = " + std::to_string((ULONG)h));

		if(m == WM_NCCREATE) {
			SetWindowLongPtr(h, GWLP_USERDATA, (LONG_PTR)((LPCREATESTRUCT)l)->lpCreateParams);
			return TRUE;
		}
		else {
			Window* window = (Window*)GetWindowLongPtr(h, GWLP_USERDATA);
			if (window) {
				return window->Processor(h, m, w, l);
			}
		}
		return DefWindowProc(h, m, w, l);
	}
	LRESULT Window::Processor(HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			KeyDown((BYTE)w);
			break;
		default:
			return DefWindowProc(h, m, w, l);
		}
		return LRESULT(0);
	}
	Window::Window(HINSTANCE hinstance)
	{
		WNDCLASS wc{ 0 };
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hInstance = hinstance;
		wc.lpszClassName = className;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = StaticProcessor;
		wc.cbWndExtra = sizeof(Window*);
		RegisterClass(&wc);

		handle = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, HWND_DESKTOP, nullptr, hinstance, this);

		ShowWindow(handle, SW_NORMAL);
	}


	Window::~Window()
	{
	}
	void Window::EnterMessageLoop()
	{
		MSG msg;
		while (GetMessage(&msg, nullptr, 0,0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}