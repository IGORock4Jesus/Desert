#pragma once

#include <Windows.h>
#include "event.h"

namespace Desert::System {
	class Window
	{
		HWND handle;
		LPCSTR className = "DESERT WINDOW";

		static LRESULT CALLBACK StaticProcessor(HWND h, UINT m, WPARAM w, LPARAM l);
		LRESULT Processor(HWND h, UINT m, WPARAM w, LPARAM l);

	public:
		Event<Window, BYTE> KeyDown;

		Window(HINSTANCE hinstance);
		~Window();

		void EnterMessageLoop();

		HWND GetHandle() const { return handle; }
		int GetWidth() const { RECT r; ::GetClientRect(handle, &r); return r.right - r.left; }
		int GetHeight() const { RECT r; ::GetClientRect(handle, &r); return r.bottom - r.top; }
	};

}